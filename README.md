# ESP8266 Roller Controller

This project controls a roller blind using an ESP8266 module. Wi-Fi and MQTT
credentials are stored in a separate configuration header to keep secrets out of
version control.

## Configuration

Update `include/Secrets.h` with your network and MQTT settings:

```cpp
const char* WIFI_SSID = "your_wifi_ssid";
const char* WIFI_PASSWORD = "your_wifi_password";
const char* MQTT_HOST = "your.mqtt.host";
const char* OTA_PASSWORD = "your_ota_password";
```

The file is listed in `.gitignore` so local changes won't be committed
accidentally.

## Building

Use [PlatformIO](https://platformio.org/) to build the project:

```bash
pio run
```

## MQTT Control

The firmware follows the Wiren Board MQTT conventions. Controls are published
under `/devices/roller_1/controls/` and accept commands on the `/on` subtopics.
You can test the device with `mosquitto_pub` (or any other MQTT client):

```bash
# Fully open the blind
mosquitto_pub -h <mqtt_host> -t /devices/roller_1/controls/open/on -m 1

# Fully close the blind
mosquitto_pub -h <mqtt_host> -t /devices/roller_1/controls/close/on -m 1

# Stop movement
mosquitto_pub -h <mqtt_host> -t /devices/roller_1/controls/stop/on -m 1

# Move to a specific position (0-100)
mosquitto_pub -h <mqtt_host> -t /devices/roller_1/controls/position/on -m 50

# Recalibrate current position as fully closed
mosquitto_pub -h <mqtt_host> -t /devices/roller_1/controls/recalibrate/on -m 1

# Calibrate current position as fully open
mosquitto_pub -h <mqtt_host> -t /devices/roller_1/controls/calibrate_open/on -m 1

# Reset stored calibration data
mosquitto_pub -h <mqtt_host> -t /devices/roller_1/controls/reset_calibration/on -m 1
```

## Over-the-Air Updates

The firmware supports OTA updates using `ArduinoOTA`. Set an OTA password in
`include/Secrets.h` and upload over the network with PlatformIO:

```bash
pio run -t upload --upload-port <device-ip> --upload-password <your_ota_password>
```

## Continuous Integration

GitHub Actions workflow `.github/workflows/ci.yml` builds the project on every
push. When repository secrets `ESP_HOST` and `ESP_PASS` are configured, the
workflow also uploads the firmware to the device via OTA.
