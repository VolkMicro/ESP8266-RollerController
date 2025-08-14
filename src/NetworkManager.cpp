#include "NetworkManager.h"
#include <Arduino.h>
#include <ArduinoOTA.h>
#include "Logger.h"

void NetworkManager::begin(MessageCallback cb) {
    callback = std::move(cb);
    auto &cfg = Settings::data();
    const char* ssid = strlen(cfg.ssid) ? cfg.ssid : WIFI_SSID;
    const char* pass = strlen(cfg.password) ? cfg.password : WIFI_PASSWORD;
    const char* mqtt = strlen(cfg.mqttHost) ? cfg.mqttHost : MQTT_HOST;
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    LOGF("Connecting to WiFi: %s\n", ssid);
    mqttClient.setServer(mqtt, 1883);
    mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
        if (length >= 100) {
            return; // payload too large
        }
        payload[length] = '\0';
        String msg((char*)payload);
        if (callback) {
            callback(topic, msg);
        }
    });
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        LOGLN("WiFi connect failed, starting config portal");
        portal.begin();
        configMode = true;
    }
}

bool NetworkManager::update() {
    if (configMode) {
        portal.handle();
        return false;
    }
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long now = millis();
        if (now - lastWifiReconnectAttempt >= 5000) {
            LOGLN("WiFi not connected");
            // Throttle reconnect attempts to once every 5 seconds
            WiFi.reconnect();
            lastWifiReconnectAttempt = now;
        }
        return false;
    }
    if (!mqttClient.connected()) {
        handleReconnect();
        return mqttClient.connected();
    }
    mqttClient.loop();
    return false;
}

void NetworkManager::handleReconnect() {
    auto &cfg = Settings::data();
    const char* mqtt = strlen(cfg.mqttHost) ? cfg.mqttHost : MQTT_HOST;
    LOGF("Connecting to MQTT at %s...\n", mqtt);
    if (mqttClient.connect("ESP8266_Roller")) {
        LOGLN("MQTT connected");
        mqttClient.subscribe(Config::TOPIC_OPEN_SET);
        mqttClient.subscribe(Config::TOPIC_CLOSE_SET);
        mqttClient.subscribe(Config::TOPIC_STOP_SET);
        mqttClient.subscribe(Config::TOPIC_POSITION_SET);
        mqttClient.subscribe(Config::TOPIC_RECALIBRATE_SET);
        mqttClient.subscribe(Config::TOPIC_RESET_CALIBRATION_SET);
        mqttClient.subscribe(Config::TOPIC_CALIBRATE_OPEN_SET);
    } else {
        LOGLN("MQTT connection failed");
    }
}

void NetworkManager::publish(const char* topic, const String& payload, bool retained) {
    mqttClient.publish(topic, payload.c_str(), retained);
}

