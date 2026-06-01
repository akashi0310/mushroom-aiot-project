#include <Arduino.h>
#include "config.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <time.h>

// ── ML classifiers (auto-generated from Python training pipeline) ─────────────
#include "plant_classifier.h"     // classifyPlantHealth() → HealthStatus
#include "actuator_classifier.h"  // classifyActuator()   → ActuatorAction

// ── Hardware pins ─────────────────────────────────────────────────────────────
// Pump relay: GPIO 14 (NodeMCU D5) – active HIGH
// Fan  relay: GPIO 12 (NodeMCU D6) – active HIGH
//   (Relay modules are usually active-LOW; invert PUMP_ON/FAN_ON if needed)
#define PUMP_PIN  14
#define FAN_PIN   12

DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

struct SensorData
{
    time_t timestamp;
    float  air_temperature;
    float  air_humidity;
    float  soil_moisture;
};

SensorData dataCache[MAX_CACHE_SIZE];
int cacheCount = 0;
unsigned long lastSampleTime = 0;

<<<<<<< HEAD
<<<<<<< HEAD
// ── Helpers ───────────────────────────────────────────────────────────────────
void syncNTPTime() {
=======
void syncNTPTime()
{
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
=======
void syncNTPTime()
{
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
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

<<<<<<< HEAD
<<<<<<< HEAD
void reconnectMQTT() {
    while (!mqttClient.connected()) {
        if (WiFi.status() != WL_CONNECTED) connectToWiFi();
        Serial.print("[MQTT] Connecting to broker...");
        String clientId = "ESP8266-" + String(random(0, 0xffff), HEX);
        if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
            Serial.println("connected!");
        } else {
            Serial.printf("failed, rc=%d  retrying in 5s...\n", mqttClient.state());
=======
=======
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
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
            Serial.println(" Trying again in 5 seconds...");
<<<<<<< HEAD
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
=======
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
            delay(5000);
        }
    }
}

<<<<<<< HEAD
<<<<<<< HEAD
// Apply actuator decision: drive relay pins and report to serial.
void applyActuatorAction(ActuatorAction action) {
    bool pump = (action == ACTUATOR_PUMP)         || (action == ACTUATOR_PUMP_AND_FAN);
    bool fan  = (action == ACTUATOR_FAN)          || (action == ACTUATOR_PUMP_AND_FAN);

    digitalWrite(PUMP_PIN, pump ? HIGH : LOW);
    digitalWrite(FAN_PIN,  fan  ? HIGH : LOW);

    Serial.printf("[ACT] pump=%s  fan=%s  (%s)\n",
                  pump ? "ON" : "OFF",
                  fan  ? "ON" : "OFF",
                  ACTUATOR_NAMES[action]);
}

// Publish sensor + actuator state as JSON to MQTT.
bool publishTelemetry(const SensorData& d, HealthStatus health, ActuatorAction action) {
    if (!mqttClient.connected()) reconnectMQTT();

    // ── environment topic ────────────────────────────────────────────────────
    {
        JsonDocument doc;
        doc["timestamp"]       = d.timestamp;
        doc["air_temperature"] = d.air_temperature;
        doc["air_humidity"]    = d.air_humidity;
        doc["soil_moisture"]   = d.soil_moisture;
        doc["health_status"]   = HEALTH_NAMES[health];

        String payload;
        serializeJson(doc, payload);
        if (!mqttClient.publish(TOPIC_ENV, payload.c_str())) {
            Serial.println("[MQTT] Env publish failed.");
            return false;
        }
        Serial.printf("[MQTT->env] %s\n", payload.c_str());
=======
=======
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
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
<<<<<<< HEAD
=======
    }
    else
    {
        Serial.println("[MQTT] Failed to publish data.");
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
    }
    else
    {
        Serial.println("[MQTT] Failed to publish data.");
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
    }

    // ── actuator topic ────────────────────────────────────────────────────────
    {
        JsonDocument doc;
        doc["timestamp"]      = d.timestamp;
        doc["actuator"]       = ACTUATOR_NAMES[action];
        doc["pump"]           = (action == ACTUATOR_PUMP || action == ACTUATOR_PUMP_AND_FAN);
        doc["fan"]            = (action == ACTUATOR_FAN  || action == ACTUATOR_PUMP_AND_FAN);

        String payload;
        serializeJson(doc, payload);
        if (!mqttClient.publish(TOPIC_ACT, payload.c_str())) {
            Serial.println("[MQTT] Actuator publish failed.");
            return false;
        }
        Serial.printf("[MQTT->act] %s\n", payload.c_str());
    }

    return true;
}

<<<<<<< HEAD
<<<<<<< HEAD
// ── Arduino lifecycle ─────────────────────────────────────────────────────────
void setup() {
    // Soil sensor power control
=======
void setup()
{
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
=======
void setup()
{
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
    pinMode(SOIL_POWER_PIN, OUTPUT);
    digitalWrite(SOIL_POWER_PIN, LOW);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(RELAY_FAN, OUTPUT);

    // relay OFF initially
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(RELAY_FAN, HIGH);

    pinMode(DHTPIN, INPUT_PULLUP);

    // Actuator relay pins – start OFF
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(FAN_PIN,  OUTPUT);
    digitalWrite(PUMP_PIN, LOW);
    digitalWrite(FAN_PIN,  LOW);

    Serial.begin(9600);
    delay(1000);

    dht.begin();

    connectToWiFi();
<<<<<<< HEAD
<<<<<<< HEAD
    if (WiFi.status() == WL_CONNECTED) syncNTPTime();
=======
=======
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
    if (WiFi.status() == WL_CONNECTED)
    {
        syncNTPTime();
    }

    // TLS encrypted but skip certificate verification
    // (ESP8266 BearSSL can't easily load CA cert at runtime)
    espClient.setInsecure();
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37

    espClient.setInsecure();   // TLS without cert validation (ESP8266 limitation)
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
}

<<<<<<< HEAD
<<<<<<< HEAD
void loop() {
    if (!mqttClient.connected()) reconnectMQTT();
=======
=======
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
void loop()
{
    if (!mqttClient.connected())
    {
        reconnectMQTT();
    }
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
    mqttClient.loop();

    unsigned long now_ms = millis();
    if (now_ms - lastSampleTime < SAMPLING_INTERVAL) return;
    lastSampleTime = now_ms;

<<<<<<< HEAD
<<<<<<< HEAD
    // ── 1. Read sensors ───────────────────────────────────────────────────────
    digitalWrite(SOIL_POWER_PIN, HIGH);
    delay(200);
    int   rawSoil      = analogRead(SOIL_ANALOG_PIN);
    float soilMoisture = map(rawSoil, 1023, 300, 0, 100);
    soilMoisture = constrain(soilMoisture, 0.0f, 100.0f);
    digitalWrite(SOIL_POWER_PIN, LOW);   // Save power
=======
=======
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
    if (currentMillis - lastSampleTime >= SAMPLING_INTERVAL)
    {
        lastSampleTime = currentMillis;
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37

    float air_h = dht.readHumidity();
    float air_t = dht.readTemperature();
    if (isnan(air_h) || isnan(air_t)) {
        Serial.println("[ERROR] Failed to read DHT11 sensor!");
        return;
    }

    // ── 2. Run ML classifiers ─────────────────────────────────────────────────
    HealthStatus  health = classifyPlantHealth(air_t, air_h, soilMoisture);
    ActuatorAction action = classifyActuator   (air_t, air_h, soilMoisture);

    Serial.printf("[ENV] %.1fC  %.1f%%RH  soil:%.1f%%  health:%s\n",
                  air_t, air_h, soilMoisture, HEALTH_NAMES[health]);

    // ── 3. Apply actuator decision ────────────────────────────────────────────
    applyActuatorAction(action);

    // ── 4. Cache & publish ────────────────────────────────────────────────────
    time_t now = time(nullptr);
    SensorData sd = { now, air_t, air_h, soilMoisture };

    // Circular cache
    if (cacheCount < MAX_CACHE_SIZE) {
        dataCache[cacheCount++] = sd;
    } else {
        for (int i = 1; i < MAX_CACHE_SIZE; i++) dataCache[i - 1] = dataCache[i];
        dataCache[MAX_CACHE_SIZE - 1] = sd;
    }

    if (WiFi.status() != WL_CONNECTED) {
        connectToWiFi();
    } else {
        if (now < 8 * 3600 * 2) syncNTPTime();

<<<<<<< HEAD
        // Flush cached readings (use the current action for each; in a richer
        // implementation you would store the action alongside each sample)
        if (cacheCount > 0) {
            bool ok = true;
            for (int i = 0; i < cacheCount && ok; i++) {
                ok = publishTelemetry(dataCache[i], health, action);
=======
        // Relay + Pump + Fan
        digitalWrite(RELAY_PIN, LOW);  // pump ON
        digitalWrite(RELAY_FAN, HIGH); // fan ON
        Serial.println("SWITCH ON");
        delay(2000);
        digitalWrite(RELAY_PIN, HIGH); // pump OFF
        digitalWrite(RELAY_FAN, LOW);  // fan OFF
        Serial.println("SWITCH OFF");
        delay(2000);

        // Relay + Pump + Fan
        digitalWrite(RELAY_PIN, LOW);  // pump ON
        digitalWrite(RELAY_FAN, HIGH); // fan ON
        Serial.println("SWITCH ON");
        delay(2000);
        digitalWrite(RELAY_PIN, HIGH); // pump OFF
        digitalWrite(RELAY_FAN, LOW);  // fan OFF
        Serial.println("SWITCH OFF");
        delay(2000);

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
>>>>>>> ed1126a1ab451007936623bf6cc4d28afa14cf37
            }
            if (ok) cacheCount = 0;
        }
    }
}