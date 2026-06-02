#ifndef CONFIG_H
#define CONFIG_H

const char *const WIFI_SSID = "Nhat Anh";
const char *const WIFI_PASSWORD = "12345679";

#define DHTPIN 2
#define DHTTYPE DHT11

// Actuator pins (relay modules)
#define PUMP_PIN D1 // GPIO  5 – Water pump
#define FAN1_PIN D0 // GPIO 16 – Cooling fan 1
#define FAN2_PIN D2 // GPIO  4 – Cooling fan 2

// Most relay boards are active-LOW: LOW = relay ON, HIGH = relay OFF.
// If your relay is active-HIGH, swap the two values below.
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

#define SOIL_POWER_PIN 0
#define SOIL_ANALOG_PIN A0

const unsigned long SAMPLING_INTERVAL = 5000;
const int MAX_CACHE_SIZE = 120;

// EMQX Cloud TLS — port 8883
const char *const MQTT_HOST = "ace2ba13.ala.asia-southeast1.emqxsl.com";
const int         MQTT_PORT = 8883;
#define MQTT_USER     "mushroom-esp8266"
#define MQTT_PASSWORD "mushroom-esp8266"

const char *const TOPIC_ENV = "mushroom-farm/rack-1/environment";
const char *const TOPIC_AI  = "mushroom-farm/rack-1/ai";
const char *const TOPIC_ACT = "mushroom-farm/rack-1/actuators";

#endif
