#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>

const char* ssid = "4RunnerOS-HUD";
const char* password = "password123";

AsyncWebServer server(80);
Adafruit_BMP280 bmp;

void setup() {
    Serial.begin(115200);

    // Initialize Filesystem
    if (!LittleFS.begin(true, "/spiffs")) {
    Serial.println("LittleFS Mount Failed!");
} else {
    Serial.println("LittleFS Mounted!");
}

    // Initialize I2C and Sensor
    Wire.begin(21, 22);
    if (!bmp.begin(0x76)) Serial.println("BMP280 not found");

    // Initialize WiFi
    WiFi.softAP(ssid, password);
    
    // Serve HTML from LittleFS
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html");
    });

    server.begin();
    Serial.println("Web server started at http://192.168.4.1");
}

void loop() {
    // Keep this clean for now
}