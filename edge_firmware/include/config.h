#ifndef CONFIG_H
#define CONFIG_H

// --- Cấu hình Mạng Wi-Fi ---
const char* const WIFI_SSID = "USTH_Student";
const char* const WIFI_PASSWORD = "usth2021!";

// --- Cấu hình Backend & Bảo mật ---
const char* const BACKEND_URL = "http://192.168.1.50:5000/api/sensor-data"; 
const char* const DEVICE_TOKEN = "MUSHROOM_EDGE_NODE_SECURE_TOKEN_2026";

// --- Cấu hình Cảm biến Không khí (DHT11) ---
#define DHTPIN 2      // Chân D4 trên NodeMCU tương ứng với GPIO2
#define DHTTYPE DHT11

// --- Cấu hình Cảm biến Đất ---
#define SOIL_POWER_PIN 0  // Chân D3 trên NodeMCU tương ứng với GPIO0 (Dùng cấp nguồn)
#define SOIL_ANALOG_PIN A0 // Chân Analog duy nhất trên ESP8266 để đọc dữ liệu

// --- Cấu hình Hệ thống ---
const unsigned long SAMPLING_INTERVAL = 30000; // 30 giây đọc một lần
const int MAX_CACHE_SIZE = 120;                // Bộ đệm lưu trữ khi mất mạng

// --- Cấu hình MQTT Broker ---
const char* const MQTT_HOST = "broker.emqx.io";
const int MQTT_PORT = 1883;

// --- Các Topic MQTT ---
const char* const TOPIC_ENV = "mushroom-farm/rack-1/environment";
const char* const TOPIC_DEVICES = "mushroom-farm/rack-1/devices";
const char* const TOPIC_AI = "mushroom-farm/rack-1/ai";
#endif