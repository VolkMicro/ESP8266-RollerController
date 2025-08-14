#include "NetworkManager.h"
#include <Arduino.h>
#include <ArduinoOTA.h>

void NetworkManager::begin(MessageCallback cb) {
    callback = std::move(cb);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.printf("Connecting to WiFi: %s\n", WIFI_SSID);
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
        Serial.println("WiFi not connected");
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
    Serial.printf("Connecting to MQTT at %s...\n", MQTT_HOST);
    if (mqttClient.connect("ESP8266_Roller")) {
        Serial.println("MQTT connected");
        mqttClient.subscribe(Config::TOPIC_OPEN_SET);
        mqttClient.subscribe(Config::TOPIC_CLOSE_SET);
        mqttClient.subscribe(Config::TOPIC_STOP_SET);
        mqttClient.subscribe(Config::TOPIC_POSITION_SET);
        mqttClient.subscribe(Config::TOPIC_RECALIBRATE_SET);
        mqttClient.subscribe(Config::TOPIC_RESET_CALIBRATION_SET);
        mqttClient.subscribe(Config::TOPIC_CALIBRATE_OPEN_SET);
    } else {
        Serial.println("MQTT connection failed");
    }
}

void NetworkManager::publish(const char* topic, const String& payload, bool retained) {
    mqttClient.publish(topic, payload.c_str(), retained);
}

