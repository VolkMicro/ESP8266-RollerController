# ESP8266 Roller Controller

This project controls a roller blind using an ESP8266 module. Wi-Fi and MQTT
credentials are supplied at build time so they don't need to be stored in
version control.

## Configuration

Before building, export the required credentials as environment variables:

```bash
export WIFI_SSID="your_wifi_ssid"
export WIFI_PASSWORD="your_wifi_password"
export MQTT_HOST="your.mqtt.host"
export OTA_PASSWORD="your_ota_password"
```

PlatformIO reads these variables and injects them into the firmware at compile
time. The secrets themselves remain outside the repository.

After the device connects to your Wi-Fi network, open its IP address in a web
browser to access the control page. The page shows live logs and provides
buttons to open, close, or stop the blind. You can also calibrate the fully
open and closed positions or set the maximum step count directly from the
browser.

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

## Local Web Interface

When the ESP8266 successfully connects to your Wi-Fi network, it serves a web
interface on port 80. Navigate to `http://<device-ip>/` to:

- View live logs emitted by the controller
- Open, close or stop the blind
- Recalibrate closed and open positions
- Set or reset the maximum step calibration

The interface also provides a simple field to enter a new `Max Steps` value for
manual calibration.

## Over-the-Air Updates

The firmware supports OTA updates using `ArduinoOTA`. Ensure the `OTA_PASSWORD`
environment variable is set and upload over the network with PlatformIO:

```bash
pio run -t upload --upload-port <device-ip> --upload-password <your_ota_password>
```

## Continuous Integration

GitHub Actions workflow `.github/workflows/ci.yml` builds the project on every
push. When repository secrets `ESP_HOST` and `ESP_PASS` are configured, the
workflow also uploads the firmware to the device via OTA.
