#include <Arduino.h>
#include "config.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <time.h>

#include "plant_classifier.h" 

DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

struct SensorData
{
    time_t timestamp;
    float air_temperature;
    float air_humidity;
    float soil_moisture;
};

SensorData dataCache[MAX_CACHE_SIZE];
int cacheCount = 0;
unsigned long lastSampleTime = 0;

// Non-blocking states for actuators
bool actuatorsActive = false;
unsigned long actuatorStartTime = 0;
const unsigned long PUMP_RUN_DURATION = 2000; // 2 seconds
const unsigned long FAN_RUN_DURATION = 4000;  // 4 seconds total action time

void syncNTPTime()
{
    Serial.println("[NTP] Synchronizing real time...");
    configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2)
    {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println("\n[NTP] Synchronization complete!");
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
    while (!mqttClient.connected())
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            connectToWiFi();
        }
        Serial.print("[MQTT] Connecting to Broker...");
        
        // Using static buffer instead of dynamic String manipulation to protect heap memory
        char clientBuf[32];
        snprintf(clientBuf, sizeof(clientBuf), "ESP8266Client-%04X", (uint16_t)random(0, 0xffff));

#if defined(MQTT_USER) && defined(MQTT_PASSWORD)
        if (mqttClient.connect(clientBuf, MQTT_USER, MQTT_PASSWORD))
#else
        if (mqttClient.connect(clientBuf))
#endif
        {
            Serial.println("Connected!");
        }
        else
        {
            Serial.print("Failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" Trying again in 5 seconds...");
            delay(5000);
        }
    }
}

bool flushCache(SensorData* dataArray, int count) {
    if (!mqttClient.connected()) {
        reconnectMQTT();
    }

    bool allOk = true;
    for (int i = 0; i < count; i++) {
        JsonDocument doc;
        doc["timestamp"]       = dataArray[i].timestamp;
        doc["air_temperature"] = dataArray[i].air_temperature;
        doc["air_humidity"]    = dataArray[i].air_humidity;
        doc["soil_moisture"]   = dataArray[i].soil_moisture;

        String payload;
        serializeJson(doc, payload);

        if (mqttClient.publish(TOPIC_ENV, payload.c_str())) {
            Serial.printf("[MQTT] Published: %s\n", payload.c_str());
        } else {
            Serial.println("[MQTT] Publish failed.");
            allOk = false;
        }
    }
    return allOk;
}

void setup()
{
    pinMode(SOIL_POWER_PIN, OUTPUT);
    digitalWrite(SOIL_POWER_PIN, LOW);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(RELAY_FAN, OUTPUT);

    // Assuming active-low relays: HIGH is OFF
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(RELAY_FAN, HIGH);

    pinMode(DHTPIN, INPUT_PULLUP);

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
    if (!mqttClient.connected())
    {
        reconnectMQTT();
    }
    mqttClient.loop();

    unsigned long currentMillis = millis();

    // 1. NON-BLOCKING SAMPLING INTERVAL
    if (currentMillis - lastSampleTime >= SAMPLING_INTERVAL)
    {
        lastSampleTime = currentMillis;

        // Turn on soil sensor briefly to read
        digitalWrite(SOIL_POWER_PIN, HIGH);
        delay(50); // Small brief window for voltage stabilization 

        int rawSoil = analogRead(SOIL_ANALOG_PIN);
        digitalWrite(SOIL_POWER_PIN, LOW); // Turn off to prevent corrosion
        
        float soilMoisture = map(rawSoil, 1023, 300, 0, 100);
        if (soilMoisture > 100) soilMoisture = 100;
        if (soilMoisture < 0)   soilMoisture = 0;

        float air_h = dht.readHumidity();
        float air_t = dht.readTemperature();

        if (isnan(air_h) || isnan(air_t)) {
            Serial.println("[ERROR] Failed to read DHT11 sensor!");
            return;
        }

        HealthStatus status = classifyPlantHealth(air_t, air_h, soilMoisture);

        Serial.printf(
            "[ENV] %.1f°C  %.1f%%  soil:%.1f%%  => %s\n",
            air_t, air_h, soilMoisture, HEALTH_NAMES[status]
        );

        time_t now = time(nullptr);

        // Trigger Actuators (Non-blocking tracking begins)
        actuatorsActive = true;
        actuatorStartTime = currentMillis;
        
        // Active-low logic: LOW turns ON
        digitalWrite(RELAY_PIN, LOW);  // Pump ON
        digitalWrite(RELAY_FAN, LOW);  // Fan ON
        Serial.println("[ACTUATORS] Pump and Fan Turned ON");

        // Array Cache management
        if (cacheCount < MAX_CACHE_SIZE) {
            dataCache[cacheCount++] = { now, air_t, air_h, soilMoisture };
        } else {
            for (int i = 1; i < MAX_CACHE_SIZE; i++) dataCache[i - 1] = dataCache[i];
            dataCache[MAX_CACHE_SIZE - 1] = { now, air_t, air_h, soilMoisture };
        }

        if (WiFi.status() != WL_CONNECTED)
        {
            connectToWiFi();
        } else {
            if (now < 8 * 3600 * 2) syncNTPTime();

            if (cacheCount > 0 && flushCache(dataCache, cacheCount)) {
                cacheCount = 0;
            }
        }
    }

    // 2. NON-BLOCKING ACTUATOR DURATIONS STATE-MACHINE
    if (actuatorsActive)
    {
        unsigned long elapsed = currentMillis - actuatorStartTime;

        // Turn off Pump after 2 seconds
        if (elapsed >= PUMP_RUN_DURATION && digitalRead(RELAY_PIN) == LOW) {
            digitalWrite(RELAY_PIN, HIGH); // Pump OFF
            Serial.println("[ACTUATORS] Pump Turned OFF");
        }

        // Turn off Fan after 4 seconds total
        if (elapsed >= FAN_RUN_DURATION) {
            digitalWrite(RELAY_FAN, HIGH); // Fan OFF
            Serial.println("[ACTUATORS] Fan Turned OFF");
            actuatorsActive = false; // Routine completed
        }
    }
}