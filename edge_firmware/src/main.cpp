#include <Arduino.h>
#include "config.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <time.h>

DHT dht(DHTPIN, DHTTYPE);

// Cấu trúc gói dữ liệu thu nhỏ (Chỉ lưu trữ các thông số đo đạc)
struct SensorData {
    time_t timestamp;
    float air_temperature;
    float air_humidity;
    float soil_moisture;
};

SensorData dataCache[MAX_CACHE_SIZE];
int cacheCount = 0;
unsigned long lastSampleTime = 0;

void syncNTPTime() {
    Serial.println("[NTP] Đang đồng bộ thời gian thực...");
    configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    time_t now = time(nullptr);
    while (now < 8 * 3600 * 2) { 
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }
    Serial.println("\n[NTP] Đồng bộ hoàn tất!");
}

void connectToWiFi() {
    if (WiFi.status() == WL_CONNECTED) return;
    Serial.print("[MẠNG] Đang kết nối Wi-Fi: ");
    Serial.println(WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 30) {
        delay(500);
        Serial.print(".");
        attempt++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[MẠNG] Đã kết nối Wi-Fi!");
    }
}

bool sendBatchData(SensorData* dataArray, int count) {
    if (WiFi.status() != WL_CONNECTED) return false;

    WiFiClient client;
    HTTPClient http;

    http.begin(client, BACKEND_URL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Device-Token", DEVICE_TOKEN);

    // --- ARDUINOJSON V7 UPDATE ---
    // Khởi tạo tài liệu cấu trúc JSON động, tự quản lý vùng nhớ tự động
    JsonDocument doc;
    JsonArray array = doc.to<JsonArray>();

    for (int i = 0; i < count; i++) {
        // --- ARDUINOJSON V7 UPDATE ---
        // Thay thế hàm lỗi thời createNestedObject() thành cú pháp add<JsonObject>() của bản V7
        JsonObject obj = array.add<JsonObject>();
        obj["timestamp"] = dataArray[i].timestamp;
        obj["air_temperature"] = dataArray[i].air_temperature;
        obj["air_humidity"] = dataArray[i].air_humidity;
        obj["soil_moisture"] = dataArray[i].soil_moisture;
    }

    String jsonPayload;
    serializeJson(doc, jsonPayload);

    int httpResponseCode = http.POST(jsonPayload);
    bool success = false;

    if (httpResponseCode == 200 || httpResponseCode == 201) {
        Serial.println("[HTTP] Đã truyền dữ liệu lên Server thành công.");
        success = true;
    } else {
        Serial.printf("[HTTP] Gửi lỗi hoặc lỗi kết nối, mã phản hồi: %d\n", httpResponseCode);
    }

    http.end();
    return success;
}

void setup() {
    Serial.begin(9600);
    dht.begin();

    // Cấu hình chân D3 làm chân cấp nguồn đầu ra cho cảm biến đất
    pinMode(SOIL_POWER_PIN, OUTPUT);
    digitalWrite(SOIL_POWER_PIN, LOW); // Ban đầu tắt đi để bảo vệ cảm biến

    connectToWiFi();
    if (WiFi.status() == WL_CONNECTED) {
        syncNTPTime();
    }
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastSampleTime >= SAMPLING_INTERVAL) {
        lastSampleTime = currentMillis;

        // --- BƯỚC 1: ĐỌC CẢM BIẾN ĐẤT (CẮM VÀO D3 & A0) ---
        digitalWrite(SOIL_POWER_PIN, HIGH); // Bật nguồn cảm biến đất qua chân D3
        delay(50); // Chờ 50ms cho điện áp trên cảm biến ổn định hẳn

        int rawSoil = analogRead(SOIL_ANALOG_PIN); // Đọc giá trị analog thô từ chân A0
        digitalWrite(SOIL_POWER_PIN, LOW);  // Đọc xong tắt ngay nguồn chân D3 để tránh điện phân rỉ sét

        // Chuyển đổi giá trị thô sang % (1023 là khô hoàn toàn, ~300 là ướt sũng)
        float soilMoisture = map(rawSoil, 1023, 300, 0, 100);
        if(soilMoisture > 100) soilMoisture = 100;
        if(soilMoisture < 0) soilMoisture = 0;

        // --- BƯỚC 2: ĐỌC CẢM BIẾN KHÔNG KHÍ (CẮM VÀO D4) ---
        float air_h = dht.readHumidity();
        float air_t = dht.readTemperature();

        // Kiểm tra lỗi cảm biến không khí
        if (isnan(air_h) || isnan(air_t)) {
            Serial.println("[LỖI] Không đọc được dữ liệu từ cảm biến DHT11 chân D4!");
            return;
        }

        time_t now = time(nullptr);
        Serial.printf("[MÔI TRƯỜNG] Khí: %.1f°C - %.1f%% | Đất: %.1f%%\n", air_t, air_h, soilMoisture);

        // --- BƯỚC 3: QUẢN LÝ BỘ ĐỆM ---
        if (cacheCount < MAX_CACHE_SIZE) {
            dataCache[cacheCount].timestamp = now;
            dataCache[cacheCount].air_temperature = air_t;
            dataCache[cacheCount].air_humidity = air_h;
            dataCache[cacheCount].soil_moisture = soilMoisture;
            cacheCount++;
        } else {
            // Bộ đệm đầy thì dịch chuyển xóa bản ghi cũ nhất
            for (int i = 1; i < MAX_CACHE_SIZE; i++) {
                dataCache[i - 1] = dataCache[i];
            }
            dataCache[MAX_CACHE_SIZE - 1].timestamp = now;
            dataCache[MAX_CACHE_SIZE - 1].air_temperature = air_t;
            dataCache[MAX_CACHE_SIZE - 1].air_humidity = air_h;
            dataCache[MAX_CACHE_SIZE - 1].soil_moisture = soilMoisture;
        }

        // --- BƯỚC 4: TRUYỀN DỮ LIỆU QUA WIFI ---
        if (WiFi.status() != WL_CONNECTED) {
            connectToWiFi();
        } else {
            if (now < 8 * 3600 * 2) {
                syncNTPTime();
            }

            if (cacheCount > 0) {
                if (sendBatchData(dataCache, cacheCount)) {
                    cacheCount = 0; // Gửi thành công thì xóa sạch bộ đệm
                }
            }
        }
    }
}