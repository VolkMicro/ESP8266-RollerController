#include "Secrets.h"

#ifndef WIFI_SSID_VAL
#error "WIFI_SSID environment variable not defined"
#endif
#ifndef WIFI_PASSWORD_VAL
#error "WIFI_PASSWORD environment variable not defined"
#endif
#ifndef MQTT_HOST_VAL
#error "MQTT_HOST environment variable not defined"
#endif
#ifndef OTA_PASSWORD_VAL
#error "OTA_PASSWORD environment variable not defined"
#endif

const char* WIFI_SSID = WIFI_SSID_VAL;
const char* WIFI_PASSWORD = WIFI_PASSWORD_VAL;
const char* MQTT_HOST = MQTT_HOST_VAL;
const char* OTA_PASSWORD = OTA_PASSWORD_VAL;
