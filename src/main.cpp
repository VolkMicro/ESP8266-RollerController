#include <Arduino.h>
#include <ArduinoOTA.h>
#include "MotorController.h"
#include "NetworkManager.h"
#include "Config.h"
#include "Secrets.h"
#include "Settings.h"

MotorController motor;
NetworkManager network;

void publishMeta() {
    network.publish(Config::META_DEVICE, "{\"driver\":\"roller\",\"title\":{\"en\":\"Roller Blind\"}}", true);
    network.publish(Config::META_POSITION, "{\"type\":\"range\",\"min\":0,\"max\":100}", true);
    network.publish(Config::META_OPEN, "{\"type\":\"pushbutton\"}", true);
    network.publish(Config::META_CLOSE, "{\"type\":\"pushbutton\"}", true);
    network.publish(Config::META_STOP, "{\"type\":\"pushbutton\"}", true);
    network.publish(Config::META_RECALIBRATE, "{\"type\":\"pushbutton\"}", true);
    network.publish(Config::META_RESET_CALIBRATION, "{\"type\":\"pushbutton\"}", true);
    network.publish(Config::META_CALIBRATE_OPEN, "{\"type\":\"pushbutton\"}", true);
    network.publish(Config::TOPIC_POSITION, String(motor.currentPositionPercent()), true);
    network.publish(Config::TOPIC_OPEN, "0", true);
    network.publish(Config::TOPIC_CLOSE, "0", true);
    network.publish(Config::TOPIC_STOP, "0", true);
    network.publish(Config::TOPIC_RECALIBRATE, "0", true);
    network.publish(Config::TOPIC_RESET_CALIBRATION, "0", true);
    network.publish(Config::TOPIC_CALIBRATE_OPEN, "0", true);
}

void handleMessage(const char* topic, const String& msg) {
    Serial.printf("MQTT message: %s => %s\n", topic, msg.c_str());
    if (strcmp(topic, Config::TOPIC_OPEN_SET) == 0) {
        motor.moveToPercent(100);
    } else if (strcmp(topic, Config::TOPIC_CLOSE_SET) == 0) {
        motor.moveToPercent(0);
    } else if (strcmp(topic, Config::TOPIC_STOP_SET) == 0) {
        motor.stop();
    } else if (strcmp(topic, Config::TOPIC_POSITION_SET) == 0) {
        int val = msg.toInt();
        if (val >= 0 && val <= 100) {
            motor.moveToPercent(val);
        } else {
            Serial.println("Invalid position value");
        }
    } else if (strcmp(topic, Config::TOPIC_RECALIBRATE_SET) == 0) {
        motor.recalibrate();
    } else if (strcmp(topic, Config::TOPIC_RESET_CALIBRATION_SET) == 0) {
        motor.resetCalibration();
    } else if (strcmp(topic, Config::TOPIC_CALIBRATE_OPEN_SET) == 0) {
        motor.calibrateOpen();
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("=== Roller Blind Controller Starting ===");
    Settings::begin();
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.begin();
    motor.begin();
    motor.setPositionCallback([](int pos) {
        network.publish(Config::TOPIC_POSITION, String(pos), true);
    });
    network.begin(handleMessage);
}

void loop() {
    ArduinoOTA.handle();
    if (network.update()) {
        publishMeta();
    }
    motor.update();
}

