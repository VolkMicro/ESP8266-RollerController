#pragma once
#include <AccelStepper.h>

namespace Config {
// MQTT topics
constexpr char TOPIC_POSITION[] = "/devices/roller_1/controls/position";
constexpr char TOPIC_OPEN[]     = "/devices/roller_1/controls/open";
constexpr char TOPIC_CLOSE[]    = "/devices/roller_1/controls/close";
constexpr char TOPIC_STOP[]     = "/devices/roller_1/controls/stop";
constexpr char TOPIC_RECALIBRATE[]      = "/devices/roller_1/controls/recalibrate";
constexpr char TOPIC_RESET_CALIBRATION[] = "/devices/roller_1/controls/reset_calibration";
constexpr char TOPIC_CALIBRATE_OPEN[]  = "/devices/roller_1/controls/calibrate_open";

constexpr char TOPIC_POSITION_SET[] = "/devices/roller_1/controls/position/on";
constexpr char TOPIC_OPEN_SET[]     = "/devices/roller_1/controls/open/on";
constexpr char TOPIC_CLOSE_SET[]    = "/devices/roller_1/controls/close/on";
constexpr char TOPIC_STOP_SET[]     = "/devices/roller_1/controls/stop/on";
constexpr char TOPIC_RECALIBRATE_SET[]      = "/devices/roller_1/controls/recalibrate/on";
constexpr char TOPIC_RESET_CALIBRATION_SET[] = "/devices/roller_1/controls/reset_calibration/on";
constexpr char TOPIC_CALIBRATE_OPEN_SET[]  = "/devices/roller_1/controls/calibrate_open/on";

constexpr char META_DEVICE[]   = "/devices/roller_1/meta";
constexpr char META_POSITION[] = "/devices/roller_1/controls/position/meta";
constexpr char META_OPEN[]     = "/devices/roller_1/controls/open/meta";
constexpr char META_CLOSE[]    = "/devices/roller_1/controls/close/meta";
constexpr char META_STOP[]     = "/devices/roller_1/controls/stop/meta";
constexpr char META_RECALIBRATE[]      = "/devices/roller_1/controls/recalibrate/meta";
constexpr char META_RESET_CALIBRATION[] = "/devices/roller_1/controls/reset_calibration/meta";
constexpr char META_CALIBRATE_OPEN[]  = "/devices/roller_1/controls/calibrate_open/meta";

// Stepper configuration
constexpr int  MOTOR_PIN_1 = D1;
constexpr int  MOTOR_PIN_2 = D2;
constexpr int  MOTOR_PIN_3 = D3;
constexpr int  MOTOR_PIN_4 = D4;
constexpr auto MOTOR_INTERFACE = AccelStepper::HALF4WIRE;
// Increased defaults for faster curtain movement
constexpr int  MOTOR_SPEED = 1000;
constexpr int  MOTOR_ACCEL = 600;
constexpr int  STEP_DELAY_US = 800;

constexpr int EEPROM_SIZE = 512;
constexpr int EEPROM_SETTINGS_OFFSET = 8;
constexpr long DEFAULT_MAX_STEPS = 4096;

// Limit switch configuration
constexpr bool USE_LIMIT_SWITCHES = false;
constexpr int LIMIT_SWITCH_OPEN_PIN  = D5;
constexpr int LIMIT_SWITCH_CLOSE_PIN = D6;
}

