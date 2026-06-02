#include <Arduino.h>
#include "config.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <time.h>

#include "plant_classifier.h"    // classifyPlantHealth() -> HealthStatus
#include "actuator_classifier.h" // classifyActuator()   -> ActuatorAction

DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

struct SensorData
{
    time_t timestamp;
    float air_temperature;
    float air_humidity;
    float soil_moisture;
    HealthStatus health;
};

SensorData dataCache[MAX_CACHE_SIZE];
int cacheCount = 0;
unsigned long lastSampleTime = 0;

void syncNTPTime()
{
    Serial.println("[NTP] Synchronizing real time...");
    configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    time_t now = time(nullptr);
    int retry = 0;
    while (now < 8 * 3600 * 2 && retry < 10)
    {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
        retry++;
    }
    if (now >= 8 * 3600 * 2) {
        Serial.println("\n[NTP] Synchronization complete!");
    } else {
        Serial.println("\n[NTP] Synchronization timeout. Will retry later.");
    }
}

void connectToWiFi()
{
    if (WiFi.status() == WL_CONNECTED)
        return;
    Serial.print("[NETWORK] Connecting to Wi-Fi: ");
    Serial.println(WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 30)
    {
        delay(500);
        Serial.print(".");
        attempt++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\n[NETWORK] Wi-Fi connected!");
    }
}

void reconnectMQTT()
{
    if (WiFi.status() != WL_CONNECTED)
        return;

    if (!mqttClient.connected())
    {
        Serial.print("[MQTT] Connecting to Broker...");

        char clientBuf[32];
        snprintf(clientBuf, sizeof(clientBuf), "ESP8266Client-%04X", (uint16_t)random(0, 0xffff));

        if (mqttClient.connect(clientBuf, MQTT_USER, MQTT_PASSWORD))
        {
            Serial.println("Connected!");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" Will retry next cycle.");
        }
    }
}

void applyActuatorAction(ActuatorAction action)
{
    bool pump = (action == ACTUATOR_PUMP) || (action == ACTUATOR_PUMP_AND_FAN);
    bool fan  = (action == ACTUATOR_FAN)  || (action == ACTUATOR_PUMP_AND_FAN);

    digitalWrite(PUMP_PIN, pump ? RELAY_ON : RELAY_OFF);
    digitalWrite(FAN1_PIN, fan  ? RELAY_ON : RELAY_OFF);
    digitalWrite(FAN2_PIN, fan  ? RELAY_ON : RELAY_OFF);

    Serial.printf("[ACT] pump=%s  fan1=%s  fan2=%s  (%s)\n",
                  pump ? "ON" : "OFF",
                  fan  ? "ON" : "OFF",
                  fan  ? "ON" : "OFF",
                  ACTUATOR_NAMES[action]);
}

bool flushCache(SensorData *dataArray, int count)
{
    if (!mqttClient.connected())
    {
        reconnectMQTT();
        if (!mqttClient.connected()) return false;
    }

    bool allOk = true;
    for (int i = 0; i < count; i++)
    {
        // Environment payload
        JsonDocument envDoc;
        envDoc["timestamp"]       = dataArray[i].timestamp;
        envDoc["air_temperature"] = dataArray[i].air_temperature;
        envDoc["air_humidity"]    = dataArray[i].air_humidity;
        envDoc["soil_moisture"]   = dataArray[i].soil_moisture;

        String envPayload;
        serializeJson(envDoc, envPayload);

        if (!mqttClient.publish(TOPIC_ENV, envPayload.c_str()))
        {
            Serial.println("[MQTT] Publish env failed. Re-aligning cache.");
            allOk = false;
            // Move untransmitted elements to the front of the cache
            int unspentCount = 0;
            for (int j = i; j < count; j++)
                dataArray[unspentCount++] = dataArray[j];
            cacheCount = unspentCount;
            return false;
        }

        // AI health status payload
        JsonDocument aiDoc;
        aiDoc["status"] = HEALTH_NAMES[dataArray[i].health];

        String aiPayload;
        serializeJson(aiDoc, aiPayload);

        if (!mqttClient.publish(TOPIC_AI, aiPayload.c_str()))
        {
            Serial.println("[MQTT] Publish ai failed.");
            allOk = false;
        }
        else
        {
            Serial.printf("[MQTT] Published: %s | status=%s\n",
                          envPayload.c_str(), HEALTH_NAMES[dataArray[i].health]);
        }
    }
    return allOk;
}

void setup()
{
    pinMode(SOIL_POWER_PIN, OUTPUT);
    digitalWrite(SOIL_POWER_PIN, LOW);
    pinMode(DHTPIN, INPUT_PULLUP);

    // Actuator relay pins - start all OFF
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, RELAY_OFF);
    pinMode(FAN1_PIN, OUTPUT);
    digitalWrite(FAN1_PIN, RELAY_OFF);
    pinMode(FAN2_PIN, OUTPUT);
    digitalWrite(FAN2_PIN, RELAY_OFF);

    Serial.begin(9600);
    delay(1000);

    dht.begin();
    connectToWiFi();

    if (WiFi.status() == WL_CONNECTED)
    {
        syncNTPTime();
    }

    espClient.setInsecure();
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
}

void loop()
{
    // 1. NON-BLOCKING CONNECTION MANAGER
    if (WiFi.status() == WL_CONNECTED)
    {
        if (!mqttClient.connected())
        {
            static unsigned long lastMqttRetry = 0;
            if (millis() - lastMqttRetry > 5000)
            {
                lastMqttRetry = millis();
                reconnectMQTT();
            }
        }
        else
        {
            mqttClient.loop();
        }
    }

    unsigned long currentMillis = millis();

    // 2. NON-BLOCKING SAMPLING INTERVAL
    if (currentMillis - lastSampleTime >= SAMPLING_INTERVAL)
    {
        lastSampleTime = currentMillis;

        digitalWrite(SOIL_POWER_PIN, HIGH);
        delay(200);
        int rawSoil = analogRead(SOIL_ANALOG_PIN);
        digitalWrite(SOIL_POWER_PIN, LOW);

        float soilMoisture = map(rawSoil, 1023, 300, 0, 100);
        if (soilMoisture > 100) soilMoisture = 100;
        if (soilMoisture < 0)   soilMoisture = 0;

        float air_h = dht.readHumidity();
        float air_t = dht.readTemperature();

        if (isnan(air_h) || isnan(air_t))
        {
            Serial.println("[ERROR] Failed to read DHT11 sensor!");
            return;
        }

        HealthStatus status = classifyPlantHealth(air_t, air_h, soilMoisture);
        ActuatorAction action = classifyActuator(air_t, air_h, soilMoisture);

        Serial.printf("[ENV] %.1fC  %.1f%%  soil:%.1f%%  => %s\n",
                      air_t, air_h, soilMoisture, HEALTH_NAMES[status]);

        applyActuatorAction(action);

        time_t now = time(nullptr);

        if (cacheCount < MAX_CACHE_SIZE)
        {
            dataCache[cacheCount++] = {now, air_t, air_h, soilMoisture, status};
            Serial.printf("[CACHE] Buffered locally. Size: %d/%d\n", cacheCount, MAX_CACHE_SIZE);
        }
        else
        {
            for (int i = 1; i < MAX_CACHE_SIZE; i++)
                dataCache[i - 1] = dataCache[i];
            dataCache[MAX_CACHE_SIZE - 1] = {now, air_t, air_h, soilMoisture, status};
            Serial.println("[CACHE] Buffer exceeded! Overwriting oldest record.");
        }
    }

    // 3. CACHE FLUSH (runs whenever online and cache has data)
    if (WiFi.status() == WL_CONNECTED && mqttClient.connected() && cacheCount > 0)
    {
        time_t now = time(nullptr);
        if (now < 8 * 3600 * 2)
            syncNTPTime();

        Serial.println("[CACHE] Connection established. Flushing...");
        if (flushCache(dataCache, cacheCount))
        {
            cacheCount = 0;
            Serial.println("[CACHE] Flush complete. Buffer reset.");
        }
    }
}
