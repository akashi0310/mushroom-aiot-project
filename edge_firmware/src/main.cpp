#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <time.h>

// ─── WiFi Configuration ─────────────────────────────────────────────────────
const char* WIFI_SSID     = "D2411";
const char* WIFI_PASSWORD = "24267872";

// ─── MQTT Configuration ─────────────────────────────────────────────────────
const char* MQTT_BROKER = "broker.emqx.io";
const int   MQTT_PORT   = 1883;
const char* MQTT_TOPIC  = "mushroom-farm/rack-1/environment";
const char* MQTT_CLIENT_ID_PREFIX = "esp8266-mushroom-";

// ─── Sensor Pins ─────────────────────────────────────────────────────────────
#define DHTPIN    D4          // DHT11 data pin → GPIO2 (D4 on NodeMCU)
#define DHTTYPE   DHT11
#define SOIL_PIN  A0          // HW-080 analog output → A0

// ─── Timing ──────────────────────────────────────────────────────────────────
const unsigned long SEND_INTERVAL_MS = 4000;  // 4 seconds
unsigned long lastSendTime = 0;

// ─── NTP Configuration (for Unix timestamp) ──────────────────────────────────
const char* NTP_SERVER = "pool.ntp.org";
const long  GMT_OFFSET = 7 * 3600;   // UTC+7 (Vietnam)
const int   DST_OFFSET = 0;

// ─── Objects ─────────────────────────────────────────────────────────────────
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ─── Function Declarations ───────────────────────────────────────────────────
void connectWiFi();
void connectMQTT();
float readSoilMoisture();
void publishSensorData();

// ═════════════════════════════════════════════════════════════════════════════
//  SETUP
// ═════════════════════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("========================================");
  Serial.println("  Mushroom Farm - Sensor Node (ESP8266)");
  Serial.println("========================================");

  // Initialize DHT11
  dht.begin();
  Serial.println("[SENSOR] DHT11 initialized on pin D4");

  // Initialize soil moisture pin
  pinMode(SOIL_PIN, INPUT);
  Serial.println("[SENSOR] HW-080 soil moisture on pin A0");

  // Connect to WiFi
  connectWiFi();

  // Configure NTP for Unix timestamps
  configTime(GMT_OFFSET, DST_OFFSET, NTP_SERVER);
  Serial.println("[NTP] Waiting for time sync...");
  while (time(nullptr) < 100000) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\n[NTP] Time synchronized!");

  // Configure MQTT
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  connectMQTT();
}

// ═════════════════════════════════════════════════════════════════════════════
//  LOOP
// ═════════════════════════════════════════════════════════════════════════════
void loop() {
  // Ensure WiFi is connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WIFI] Connection lost! Reconnecting...");
    connectWiFi();
  }

  // Ensure MQTT is connected
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  // Publish sensor data at interval
  unsigned long now = millis();
  if (now - lastSendTime >= SEND_INTERVAL_MS) {
    lastSendTime = now;
    publishSensorData();
  }
}

// ═════════════════════════════════════════════════════════════════════════════
//  WiFi Connection
// ═════════════════════════════════════════════════════════════════════════════
void connectWiFi() {
  Serial.printf("[WIFI] Connecting to %s", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.printf("[WIFI] Connected! IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println();
    Serial.println("[WIFI] Failed to connect. Restarting...");
    ESP.restart();
  }
}

// ═════════════════════════════════════════════════════════════════════════════
//  MQTT Connection
// ═════════════════════════════════════════════════════════════════════════════
void connectMQTT() {
  // Generate unique client ID
  String clientId = MQTT_CLIENT_ID_PREFIX + String(ESP.getChipId(), HEX);

  while (!mqttClient.connected()) {
    Serial.printf("[MQTT] Connecting to %s:%d ...\n", MQTT_BROKER, MQTT_PORT);

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("[MQTT] Connected!");
      Serial.printf("[MQTT] Publishing to: %s\n", MQTT_TOPIC);
    } else {
      Serial.printf("[MQTT] Failed, rc=%d. Retrying in 3s...\n", mqttClient.state());
      delay(3000);
    }
  }
}

// ═════════════════════════════════════════════════════════════════════════════
//  Read Soil Moisture (HW-080)
// ═════════════════════════════════════════════════════════════════════════════
float readSoilMoisture() {
  // HW-080 on ESP8266 A0: 0-1023
  // Dry soil → high analog value (~1023)
  // Wet soil → low analog value (~0)
  int rawValue = analogRead(SOIL_PIN);

  // Convert to percentage: 0% = dry, 100% = wet
  float moisture = 100.0 - (rawValue / 1023.0 * 100.0);

  // Clamp to valid range
  if (moisture < 0.0) moisture = 0.0;
  if (moisture > 100.0) moisture = 100.0;

  return moisture;
}

// ═════════════════════════════════════════════════════════════════════════════
//  Publish Sensor Data via MQTT
// ═════════════════════════════════════════════════════════════════════════════
void publishSensorData() {
  // Read DHT11
  float temperature = dht.readTemperature();    // °C
  float humidity    = dht.readHumidity();        // %

  // Validate DHT11 readings
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("[SENSOR] DHT11 read failed! Skipping this cycle.");
    return;
  }

  // Read soil moisture
  float soilMoisture = readSoilMoisture();

  // Get Unix timestamp
  time_t timestamp = time(nullptr);

  // Build JSON payload matching backend EnvironmentPayload schema:
  // { "timestamp": <unix_epoch>, "air_temperature": <float>,
  //   "air_humidity": <float>, "soil_moisture": <float> }
  JsonDocument doc;
  doc["timestamp"]       = (unsigned long)timestamp;
  doc["air_temperature"] = round(temperature * 10.0) / 10.0;  // 1 decimal
  doc["air_humidity"]    = round(humidity * 10.0) / 10.0;
  doc["soil_moisture"]   = round(soilMoisture * 10.0) / 10.0;

  char payload[256];
  serializeJson(doc, payload, sizeof(payload));

  // Publish
  if (mqttClient.publish(MQTT_TOPIC, payload)) {
    Serial.printf("[MQTT] Published: %s\n", payload);
  } else {
    Serial.println("[MQTT] Publish failed!");
  }
}