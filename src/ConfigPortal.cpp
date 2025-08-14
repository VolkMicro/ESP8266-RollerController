#include "ConfigPortal.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <Arduino.h>
#include "Settings.h"
#include "Config.h"
#include "Logger.h"

void ConfigPortal::begin() {
    WiFi.softAP("RollerSetup");
    server.on("/", [this]() {
        String page =
            "<html><body><h1>Setup</h1><form method='POST' action='/save'>"
            "WiFi SSID:<input name='ssid'><br>"
            "WiFi Password:<input name='pass' type='password'><br>"
            "MQTT Host:<input name='mqtt'><br>"
            "Max Steps:<input name='steps'><br>"
            "<input type='submit' value='Save'>"
            "</form></body></html>";
        server.send(200, "text/html", page);
    });
    server.on("/save", [this]() {
        auto &cfg = Settings::data();
        if (server.hasArg("ssid")) server.arg("ssid").toCharArray(cfg.ssid, sizeof(cfg.ssid));
        if (server.hasArg("pass")) server.arg("pass").toCharArray(cfg.password, sizeof(cfg.password));
        if (server.hasArg("mqtt")) server.arg("mqtt").toCharArray(cfg.mqttHost, sizeof(cfg.mqttHost));
        Settings::save();
        if (server.hasArg("steps")) {
            long steps = server.arg("steps").toInt();
            if (steps > 0) {
                EEPROM.put(sizeof(int), steps);
                EEPROM.commit();
            }
        }
        server.send(200, "text/plain", "Saved. Rebooting...");
        delay(1000);
        ESP.restart();
    });
    server.begin();
    LOGLN("Config portal started. Connect to AP 'RollerSetup'");
}

void ConfigPortal::handle() {
    server.handleClient();
}

