#ifndef CONFIG_H
#define CONFIG_H

const char *const WIFI_SSID = "USTH_Student";
const char *const WIFI_PASSWORD = "usth2021!";

const char *const BACKEND_URL = "http://192.168.1.50:5000/api/sensor-data";
const char *const DEVICE_TOKEN = "MUSHROOM_EDGE_NODE_SECURE_TOKEN_2026";

#define DHTPIN 2
#define DHTTYPE DHT11
#define RELAY_PIN D1
#define RELAY_FAN D2

#define SOIL_POWER_PIN 0
#define SOIL_ANALOG_PIN A0

const unsigned long SAMPLING_INTERVAL = 5000;
const int MAX_CACHE_SIZE = 120;

const char *const MQTT_HOST = "broker.emqx.io";
const int MQTT_PORT = 1883;

const char *const TOPIC_ENV = "mushroom-farm/rack-1/environment";
const char *const TOPIC_DEVICES = "mushroom-farm/rack-1/devices";
const char *const TOPIC_AI = "mushroom-farm/rack-1/ai";
#endif