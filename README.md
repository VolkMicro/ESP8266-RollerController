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
```

The file is listed in `.gitignore` so local changes won't be committed
accidentally.

## Building

Use [PlatformIO](https://platformio.org/) to build the project:

```bash
pio run
```
