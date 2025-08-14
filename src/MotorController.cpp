#include "MotorController.h"
#include <Arduino.h>
#include "Logger.h"

void MotorController::begin() {
    stepper.setMaxSpeed(Config::MOTOR_SPEED);
    stepper.setAcceleration(Config::MOTOR_ACCEL);
    EEPROM.begin(Config::EEPROM_SIZE);
    EEPROM.get(0, currentPosPercent);
    EEPROM.get(sizeof(currentPosPercent), maxSteps);
    if (currentPosPercent < 0 || currentPosPercent > 100) {
        currentPosPercent = 0;
    }
    if (maxSteps <= 0) {
        maxSteps = Config::DEFAULT_MAX_STEPS;
    }
    long savedSteps = map(currentPosPercent, 0, 100, 0, maxSteps);
    stepper.setCurrentPosition(savedSteps);
    LOGF("Loaded position: %d%% (%ld/%ld steps)\n", currentPosPercent, savedSteps, maxSteps);
}

void MotorController::update() {
    if (currentState == RollerState::Moving) {
        stepper.run();
        long cur = stepper.currentPosition();
        LOGF("Step %ld/%ld (%d%%)\n", cur, stepper.targetPosition(), (int)map(cur, 0, maxSteps, 0, 100));
        if (stepper.distanceToGo() == 0) {
            currentState = RollerState::Idle;
            currentPosPercent = map(cur, 0, maxSteps, 0, 100);
            EEPROM.put(0, currentPosPercent);
            EEPROM.commit();
            LOGF("Movement complete: %d%% (%ld steps)\n", currentPosPercent, cur);
            if (positionCb) {
                positionCb(currentPosPercent);
            }
        }
    }
}

void MotorController::moveToPercent(int percent) {
    long targetSteps = map(percent, 0, 100, 0, maxSteps);
    LOGF("Move request: %d%% -> %ld steps (current %ld)\n", percent, targetSteps, stepper.currentPosition());
    stepper.moveTo(targetSteps);
    currentState = RollerState::Moving;
}

void MotorController::stop() {
    stepper.stop();
    currentState = RollerState::Idle;
    LOGF("Movement stopped at %ld steps (%d%%)\n", stepper.currentPosition(), (int)map(stepper.currentPosition(), 0, maxSteps, 0, 100));
}

void MotorController::recalibrate() {
    stepper.setCurrentPosition(0);
    currentPosPercent = 0;
    EEPROM.put(0, currentPosPercent);
    EEPROM.put(sizeof(currentPosPercent), maxSteps);
    EEPROM.commit();
    LOGLN("Calibrated closed position");
    if (positionCb) {
        positionCb(currentPosPercent);
    }
}

void MotorController::calibrateOpen() {
    maxSteps = stepper.currentPosition();
    if (maxSteps < 0) {
        maxSteps = 0;
    }
    currentPosPercent = 100;
    EEPROM.put(0, currentPosPercent);
    EEPROM.put(sizeof(currentPosPercent), maxSteps);
    EEPROM.commit();
    LOGF("Calibrated open position: %ld steps\n", maxSteps);
    if (positionCb) {
        positionCb(currentPosPercent);
    }
}

void MotorController::resetCalibration() {
    int invalid = -1;
    long invalidLong = -1;
    EEPROM.put(0, invalid);
    EEPROM.put(sizeof(currentPosPercent), invalidLong);
    EEPROM.commit();
    stepper.setCurrentPosition(0);
    currentPosPercent = 0;
    maxSteps = Config::DEFAULT_MAX_STEPS;
    LOGLN("Calibration data reset");
    if (positionCb) {
        positionCb(currentPosPercent);
    }
}


void MotorController::setMaxSteps(long steps) {
    if (steps > 0) {
        maxSteps = steps;
        EEPROM.put(sizeof(currentPosPercent), maxSteps);
        EEPROM.commit();
        LOGF("Max steps set to %ld\n", maxSteps);
    }
}
