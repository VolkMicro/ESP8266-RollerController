#include "MotorController.h"
#include <Arduino.h>

void MotorController::begin() {
    stepper.setMaxSpeed(Config::MOTOR_SPEED);
    stepper.setAcceleration(Config::MOTOR_ACCEL);
    EEPROM.begin(Config::EEPROM_SIZE);
    EEPROM.get(0, currentPosPercent);
    if (currentPosPercent < 0 || currentPosPercent > 100) {
        currentPosPercent = 0;
    }
    long savedSteps = map(currentPosPercent, 0, 100, 0, 4096);
    stepper.setCurrentPosition(savedSteps);
}

void MotorController::update() {
    if (currentState == RollerState::Moving) {
        stepper.run();
        if (stepper.distanceToGo() == 0) {
            currentState = RollerState::Idle;
            currentPosPercent = map(stepper.currentPosition(), 0, 4096, 0, 100);
            EEPROM.put(0, currentPosPercent);
            EEPROM.commit();
            if (positionCb) {
                positionCb(currentPosPercent);
            }
        }
    }
}

void MotorController::moveToPercent(int percent) {
    long targetSteps = map(percent, 0, 100, 0, 4096);
    stepper.moveTo(targetSteps);
    currentState = RollerState::Moving;
}

void MotorController::stop() {
    stepper.stop();
    currentState = RollerState::Idle;
}

void MotorController::recalibrate() {
    stepper.setCurrentPosition(0);
    currentPosPercent = 0;
    EEPROM.put(0, currentPosPercent);
    EEPROM.commit();
    if (positionCb) {
        positionCb(currentPosPercent);
    }
}

void MotorController::resetCalibration() {
    int invalid = -1;
    EEPROM.put(0, invalid);
    EEPROM.commit();
    stepper.setCurrentPosition(0);
    currentPosPercent = 0;
    if (positionCb) {
        positionCb(currentPosPercent);
    }
}

