#pragma once
#include <ESP8266WebServer.h>
#include "MotorController.h"

class WebInterface {
public:
    explicit WebInterface(MotorController& m);
    void begin();
    void handle();
private:
    ESP8266WebServer server;
    MotorController& motor;
};
