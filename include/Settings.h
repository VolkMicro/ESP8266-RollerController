#pragma once
#include <Arduino.h>

struct SettingsData {
    char ssid[32];
    char password[32];
    char mqttHost[64];
};

namespace Settings {
    void begin();
    SettingsData &data();
    void save();
}

