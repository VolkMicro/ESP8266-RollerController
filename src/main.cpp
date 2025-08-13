#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <AccelStepper.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include "Secrets.h"

// MQTT topic helpers
#define DEVICE_PREFIX    "/devices/roller_1"
#define CONTROLS_PREFIX  DEVICE_PREFIX "/controls/"

// State topics
#define TOPIC_POSITION   CONTROLS_PREFIX "position"
#define TOPIC_OPEN       CONTROLS_PREFIX "open"
#define TOPIC_CLOSE      CONTROLS_PREFIX "close"
#define TOPIC_STOP       CONTROLS_PREFIX "stop"

// Command topics
#define TOPIC_POSITION_SET TOPIC_POSITION "/on"
#define TOPIC_OPEN_SET     TOPIC_OPEN "/on"
#define TOPIC_CLOSE_SET    TOPIC_CLOSE "/on"
#define TOPIC_STOP_SET     TOPIC_STOP "/on"

// Meta topics
#define META_DEVICE        DEVICE_PREFIX "/meta"
#define META_POSITION      TOPIC_POSITION "/meta"
#define META_OPEN          TOPIC_OPEN "/meta"
#define META_CLOSE         TOPIC_CLOSE "/meta"
#define META_STOP          TOPIC_STOP "/meta"

// Stepper config
#define MOTOR_PIN_1 D1
#define MOTOR_PIN_2 D2
#define MOTOR_PIN_3 D3
#define MOTOR_PIN_4 D4
#define MOTOR_INTERFACE AccelStepper::HALF4WIRE
#define MOTOR_SPEED 500
#define MOTOR_ACCEL 300
#define STEP_DELAY_US 1200

#define EEPROM_SIZE 4

// Uncomment to enable limit switch homing
// #define USE_LIMIT_SWITCHES

#ifdef USE_LIMIT_SWITCHES
#define LIMIT_SWITCH_OPEN_PIN D5
#define LIMIT_SWITCH_CLOSE_PIN D6
#endif

AccelStepper stepper(MOTOR_INTERFACE, MOTOR_PIN_1, MOTOR_PIN_3, MOTOR_PIN_2, MOTOR_PIN_4);

WiFiClient espClient;
PubSubClient client(espClient);

int currentPosPercent = 0;
bool moving = false;

void setup_wifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void publishMeta() {
  // Device meta
  client.publish(META_DEVICE, "{\"driver\":\"roller\",\"title\":{\"en\":\"Roller Blind\"}}", true);

  // Control meta
  client.publish(META_POSITION, "{\"type\":\"range\",\"min\":0,\"max\":100}", true);
  client.publish(META_OPEN, "{\"type\":\"pushbutton\"}", true);
  client.publish(META_CLOSE, "{\"type\":\"pushbutton\"}", true);
  client.publish(META_STOP, "{\"type\":\"pushbutton\"}", true);

  // Initial states
  client.publish(TOPIC_POSITION, String(currentPosPercent).c_str(), true);
  client.publish(TOPIC_OPEN, "0", true);
  client.publish(TOPIC_CLOSE, "0", true);
  client.publish(TOPIC_STOP, "0", true);
}

void moveToPercent(int percent) {
  Serial.printf("Moving to %d%%\n", percent);
  long targetSteps = map(percent, 0, 100, 0, 4096);
  stepper.moveTo(targetSteps);
  moving = true;
}

#ifdef USE_LIMIT_SWITCHES
void homeStepper() {
  Serial.println("Homing...");
  pinMode(LIMIT_SWITCH_OPEN_PIN, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH_CLOSE_PIN, INPUT_PULLUP);
  stepper.moveTo(-10000);
  while (digitalRead(LIMIT_SWITCH_CLOSE_PIN) == HIGH) {
    stepper.run();
    ArduinoOTA.handle();
  }
  stepper.stop();
  stepper.setCurrentPosition(0);
  currentPosPercent = 0;
  EEPROM.put(0, currentPosPercent);
  EEPROM.commit();
  Serial.println("Homing complete");
}
#endif

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String msg = String((char*)payload);
  Serial.printf("MQTT Message: [%s] %s\n", topic, msg.c_str());

  if (String(topic) == TOPIC_OPEN_SET) {
    moveToPercent(100);
  } else if (String(topic) == TOPIC_CLOSE_SET) {
    moveToPercent(0);
  } else if (String(topic) == TOPIC_STOP_SET) {
    Serial.println("Stop command received");
    stepper.stop();
    moving = false;
  } else if (String(topic) == TOPIC_POSITION_SET) {
    int val = msg.toInt();
    if (val >= 0 && val <= 100) {
      moveToPercent(val);
    } else {
      Serial.println("Invalid position value");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266_Roller")) {
      Serial.println("connected");
      client.subscribe(TOPIC_OPEN_SET);
      client.subscribe(TOPIC_CLOSE_SET);
      client.subscribe(TOPIC_STOP_SET);
      client.subscribe(TOPIC_POSITION_SET);
      publishMeta();
    } else {
      Serial.printf("failed, rc=%d try again in 5 seconds\n", client.state());
      for (int i = 0; i < 10; i++) {
        ArduinoOTA.handle();
        delay(500);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("=== Roller Blind Controller Starting ===");
  setup_wifi();

  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.begin();
  Serial.println("OTA ready");

  client.setServer(MQTT_HOST, 1883);
  client.setCallback(callback);
  stepper.setMaxSpeed(MOTOR_SPEED);
  stepper.setAcceleration(MOTOR_ACCEL);
  Serial.println("Stepper initialized");

  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, currentPosPercent);
  if (currentPosPercent < 0 || currentPosPercent > 100) {
    currentPosPercent = 0;
  }
  long savedSteps = map(currentPosPercent, 0, 100, 0, 4096);
  stepper.setCurrentPosition(savedSteps);

#ifdef USE_LIMIT_SWITCHES
  homeStepper();
#endif
}

void loop() {
  ArduinoOTA.handle();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (moving) {
    stepper.run();
    if (stepper.distanceToGo() == 0) {
      moving = false;
      currentPosPercent = map(stepper.currentPosition(), 0, 4096, 0, 100);
      Serial.printf("Reached position: %d%%\n", currentPosPercent);
      client.publish(TOPIC_POSITION, String(currentPosPercent).c_str(), true);
      EEPROM.put(0, currentPosPercent);
      EEPROM.commit();
    }
  }
}
