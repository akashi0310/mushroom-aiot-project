#include <Arduino.h>
#include "config.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <time.h>

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
        String clientId = "ESP8266Client-" + String(random(0, 0xffff), HEX);

#if defined(MQTT_USER) && defined(MQTT_PASSWORD)
        if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD))
        {
#else
        if (mqttClient.connect(clientId.c_str()))
        {
#endif
            Serial.println("Connected!");
        }
        else
        {
            Serial.print("Failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" retrying in 5s...");
            delay(5000);
        }
    }
}

bool sendBatchData(SensorData *dataArray, int count)
{
    if (!mqttClient.connected())
    {
        reconnectMQTT();
    }

    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();

    for (int i = 0; i < count; i++)
    {
        JsonObject obj = array.add<JsonObject>();
        obj["timestamp"] = dataArray[i].timestamp;
        obj["air_temperature"] = dataArray[i].air_temperature;
        obj["air_humidity"] = dataArray[i].air_humidity;
        obj["soil_moisture"] = dataArray[i].soil_moisture;
    }

        String payload;
        serializeJson(doc, payload);

    bool success = mqttClient.publish(TOPIC_AI, jsonPayload.c_str());

    if (success)
    {
        Serial.println("[MQTT] Data published to Broker successfully.");
    }
    else
    {
        Serial.println("[MQTT] Failed to publish data.");
    }
    return allOk;
}

void setup()
{
    pinMode(SOIL_POWER_PIN, OUTPUT);
    digitalWrite(SOIL_POWER_PIN, LOW);
    pinMode(DHTPIN, INPUT_PULLUP);

    Serial.begin(9600);
    delay(1000);

    dht.begin();

    connectToWiFi();
    if (WiFi.status() == WL_CONNECTED)
    {
        syncNTPTime();
    }

    // TLS encrypted but skip certificate verification
    // (ESP8266 BearSSL can't easily load CA cert at runtime)
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

    if (currentMillis - lastSampleTime >= SAMPLING_INTERVAL)
    {
        lastSampleTime = currentMillis;

        digitalWrite(SOIL_POWER_PIN, HIGH);
        delay(200);

        int rawSoil = analogRead(SOIL_ANALOG_PIN);
        float soilMoisture = map(rawSoil, 1023, 300, 0, 100);
        if (soilMoisture > 100) soilMoisture = 100;
        if (soilMoisture < 0)   soilMoisture = 0;

        float air_h = dht.readHumidity();
        float air_t = dht.readTemperature();

        if (isnan(air_h) || isnan(air_t)) {
            Serial.println("[ERROR] Failed to read DHT11 sensor!");
            return;
        }

        time_t now = time(nullptr);
        Serial.printf("[ENV] %.1f°C  %.1f%%  soil:%.1f%%\n", air_t, air_h, soilMoisture);

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
}
