#ifndef CONFIG_H
#define CONFIG_H

const char* const WIFI_SSID     = "USTH_Student";
const char* const WIFI_PASSWORD = "usth2021!";

#define DHTPIN 2
#define DHTTYPE DHT11

#define SOIL_POWER_PIN  0
#define SOIL_ANALOG_PIN A0

const unsigned long SAMPLING_INTERVAL = 5000;
const int           MAX_CACHE_SIZE    = 20;

// EMQX Cloud TLS — port 8883
const char* const MQTT_HOST = "ace2ba13.ala.asia-southeast1.emqxsl.com";
const int         MQTT_PORT = 8883;
#define MQTT_USER     "mushroom-esp8266"
#define MQTT_PASSWORD "mushroom-esp8266"

const char* const TOPIC_ENV = "mushroom-farm/rack-1/environment";

#endif
