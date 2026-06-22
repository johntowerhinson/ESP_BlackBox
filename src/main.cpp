#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Adafruit_BMP280.h>
#include <MPU9250_WE.h> // Ensure you use this header
#include <TinyGPS++.h>
#include <ArduinoJson.h>
#include <Wire.h>

AsyncWebServer server(80);
Adafruit_BMP280 bmp;
MPU9250_WE mpu = MPU9250_WE(0x68); // Usually 0x68 or 0x69
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
    if (!LittleFS.begin(true)) { 
    Serial.println("LittleFS Mount Failed!");
} else {
    Serial.println("LittleFS Mounted!");
}
    Wire.begin(21, 22);

    bmp.begin(0x76);
    mpu.init(); // Correct initialization for MPU9250_WE
    mpu.initMagnetometer();
    mpu.autoOffsets(); // Highly recommended for MPU9250_WE

    WiFi.softAP("ESP_BlackBox", "password123");

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html");
    });
    
    server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    JsonDocument doc;
    
    // BMP280
    doc["temp"] = bmp.readTemperature();
    doc["press"] = bmp.readPressure() / 100.0F;
    
    // MPU9250_WE (9-Axis)
    xyzFloat acc = mpu.getGValues();
    xyzFloat gyr = mpu.getGyrValues();
    xyzFloat mag = mpu.getMagValues();
    doc["accX"] = acc.x; doc["accY"] = acc.y; doc["accZ"] = acc.z;
    doc["gyrX"] = gyr.x; doc["gyrY"] = gyr.y; doc["gyrZ"] = gyr.z;
    doc["magX"] = mag.x; doc["magY"] = mag.y; doc["magZ"] = mag.z;
    
    // GPS
    doc["lat"] = gps.location.lat();
    doc["lng"] = gps.location.lng();
    doc["time"] = String(gps.time.hour()) + ":" + String(gps.time.minute());
    
    String output;
    serializeJson(doc, output);
    request->send(200, "application/json", output);
});

    server.begin();
}

void loop() {
    while (gpsSerial.available() > 0) gps.encode(gpsSerial.read());
}