#include "Settings.h"
#include <EEPROM.h>
#include <string.h>
#include "Config.h"

static SettingsData settings;

void Settings::begin() {
    EEPROM.begin(Config::EEPROM_SIZE);
    EEPROM.get(Config::EEPROM_SETTINGS_OFFSET, settings);
    if (settings.ssid[0] == 0xFF || settings.ssid[0] == '\0') {
        memset(&settings, 0, sizeof(settings));
    }
}

SettingsData &Settings::data() {
    return settings;
}

void Settings::save() {
    EEPROM.put(Config::EEPROM_SETTINGS_OFFSET, settings);
    EEPROM.commit();
}

