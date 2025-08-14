#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <functional>
#include "Secrets.h"
#include "Config.h"

class NetworkManager {
public:
    using MessageCallback = std::function<void(const char*, const String&)>;
    void begin(MessageCallback cb);
    bool update(); // returns true if newly connected to MQTT
    void publish(const char* topic, const String& payload, bool retained = false);
    bool connected() { return mqttClient.connected(); }

private:
    void handleReconnect();
    WiFiClient wifiClient;
    PubSubClient mqttClient{wifiClient};
    MessageCallback callback;
    unsigned long lastWifiReconnectAttempt = 0;
};
