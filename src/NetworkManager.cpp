#include "NetworkManager.h"
#include <Arduino.h>
#include <ArduinoOTA.h>

void NetworkManager::begin(MessageCallback cb) {
    callback = std::move(cb);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    mqttClient.setServer(MQTT_HOST, 1883);
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
}

bool NetworkManager::update() {
    if (WiFi.status() != WL_CONNECTED) {
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
    if (mqttClient.connect("ESP8266_Roller")) {
        mqttClient.subscribe(Config::TOPIC_OPEN_SET);
        mqttClient.subscribe(Config::TOPIC_CLOSE_SET);
        mqttClient.subscribe(Config::TOPIC_STOP_SET);
        mqttClient.subscribe(Config::TOPIC_POSITION_SET);
        mqttClient.subscribe(Config::TOPIC_RECALIBRATE_SET);
        mqttClient.subscribe(Config::TOPIC_RESET_CALIBRATION_SET);
    }
}

void NetworkManager::publish(const char* topic, const String& payload, bool retained) {
    mqttClient.publish(topic, payload.c_str(), retained);
}

