#pragma once
#include <ESP8266WebServer.h>

class ConfigPortal {
public:
    void begin();
    void handle();
private:
    ESP8266WebServer server{80};
};

