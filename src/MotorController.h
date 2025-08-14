#pragma once
#include <AccelStepper.h>
#include <EEPROM.h>
#include <functional>
#include "Config.h"

enum class RollerState { Idle, Moving };

class MotorController {
public:
    using PositionCallback = std::function<void(int)>;
    void begin();
    void update();
    void moveToPercent(int percent);
    void stop();
    void recalibrate();
    void calibrateOpen();
    void resetCalibration();
    void setMaxSteps(long steps);
    void setSpeed(int speed, int accel);
    long getMaxSteps() const { return maxSteps; }
    int  currentPositionPercent() const { return currentPosPercent; }
    RollerState state() const { return currentState; }
    void setPositionCallback(PositionCallback cb) { positionCb = std::move(cb); }

private:
    AccelStepper stepper{Config::MOTOR_INTERFACE, Config::MOTOR_PIN_1, Config::MOTOR_PIN_3, Config::MOTOR_PIN_2, Config::MOTOR_PIN_4};
    RollerState currentState{RollerState::Idle};
    int currentPosPercent{0};
    long maxSteps{Config::DEFAULT_MAX_STEPS};
    PositionCallback positionCb;
};
