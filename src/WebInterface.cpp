#include "WebInterface.h"
#include <EEPROM.h>
#include "Logger.h"

WebInterface::WebInterface(MotorController& m) : server(80), motor(m) {}

void WebInterface::begin() {
    server.on("/", [this]() {
        String page =
            "<html><body><h1>Roller Control</h1>"
            "<button onclick=\"fetch('/open')\">Open</button>"
            "<button onclick=\"fetch('/close')\">Close</button>"
            "<button onclick=\"fetch('/stop')\">Stop</button>"
            "<button onclick=\"fetch('/recalibrate')\">Set Closed</button>"
            "<button onclick=\"fetch('/calibrate_open')\">Set Open</button>"
            "<button onclick=\"fetch('/reset_calibration')\">Reset Calibration</button>"
            "<h2>Calibration</h2>"
            "<input id='steps' type='number' placeholder='Max Steps'>"
            "<button onclick=\"fetch('/set_steps?steps='+document.getElementById('steps').value)\">Save</button>"
            "<h2>Speed</h2>"
            "<input id='speed' type='number' placeholder='Speed'>"
            "<input id='accel' type='number' placeholder='Acceleration'>"
            "<button onclick=\"fetch('/set_speed?speed='+document.getElementById('speed').value+'&accel='+document.getElementById('accel').value)\">Save</button>"
            "<pre id='logs'></pre>"
            "<script>async function upd(){document.getElementById('logs').textContent=await (await fetch('/logs')).text();}setInterval(upd,1000);upd();</script>"
            "</body></html>";
        server.send(200, "text/html", page);
    });
    server.on("/open", [this]() { motor.moveToPercent(100); server.send(200, "text/plain", "OK"); });
    server.on("/close", [this]() { motor.moveToPercent(0); server.send(200, "text/plain", "OK"); });
    server.on("/stop", [this]() { motor.stop(); server.send(200, "text/plain", "OK"); });
    server.on("/recalibrate", [this]() { motor.recalibrate(); server.send(200, "text/plain", "OK"); });
    server.on("/calibrate_open", [this]() { motor.calibrateOpen(); server.send(200, "text/plain", "OK"); });
    server.on("/reset_calibration", [this]() { motor.resetCalibration(); server.send(200, "text/plain", "OK"); });
    server.on("/set_steps", [this]() {
        if (server.hasArg("steps")) {
            long steps = server.arg("steps").toInt();
            motor.setMaxSteps(steps);
        }
        server.send(200, "text/plain", "OK");
    });
    server.on("/set_speed", [this]() {
        if (server.hasArg("speed") && server.hasArg("accel")) {
            int speed = server.arg("speed").toInt();
            int accel = server.arg("accel").toInt();
            motor.setSpeed(speed, accel);
        }
        server.send(200, "text/plain", "OK");
    });
    server.on("/logs", [this]() { server.send(200, "text/plain", Logger::get()); });
    server.begin();
}

void WebInterface::handle() {
    server.handleClient();
}
