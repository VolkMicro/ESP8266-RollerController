#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <AccelStepper.h>
#include "Secrets.h"

// MQTT topics
#define TOPIC_POSITION   "/devices/roller_1/controls/position"
#define TOPIC_OPEN       "/devices/roller_1/controls/open"
#define TOPIC_CLOSE      "/devices/roller_1/controls/close"
#define TOPIC_STOP       "/devices/roller_1/controls/stop"
#define META_PREFIX      "/devices/roller_1/meta/"

// Stepper config
#define MOTOR_PIN_1 D1
#define MOTOR_PIN_2 D2
#define MOTOR_PIN_3 D3
#define MOTOR_PIN_4 D4
#define MOTOR_INTERFACE AccelStepper::HALF4WIRE
#define MOTOR_SPEED 500
#define MOTOR_ACCEL 300
#define STEP_DELAY_US 1200

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
  client.publish((META_PREFIX "name"), "Roller Blind");
  client.publish((META_PREFIX "position/type"), "range[0:100]");
  client.publish((META_PREFIX "open/type"), "pushbutton");
  client.publish((META_PREFIX "close/type"), "pushbutton");
  client.publish((META_PREFIX "stop/type"), "pushbutton");
}

void moveToPercent(int percent) {
  Serial.printf("Moving to %d%%\n", percent);
  long targetSteps = map(percent, 0, 100, 0, 4096);
  stepper.moveTo(targetSteps);
  moving = true;
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String msg = String((char*)payload);
  Serial.printf("MQTT Message: [%s] %s\n", topic, msg.c_str());

  if (String(topic) == TOPIC_OPEN) {
    moveToPercent(100);
  } else if (String(topic) == TOPIC_CLOSE) {
    moveToPercent(0);
  } else if (String(topic) == TOPIC_STOP) {
    Serial.println("Stop command received");
    stepper.stop();
    moving = false;
  } else if (String(topic) == TOPIC_POSITION) {
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
      client.subscribe(TOPIC_OPEN);
      client.subscribe(TOPIC_CLOSE);
      client.subscribe(TOPIC_STOP);
      client.subscribe(TOPIC_POSITION);
      publishMeta();
    } else {
      Serial.printf("failed, rc=%d try again in 5 seconds\n", client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("=== Roller Blind Controller Starting ===");
  setup_wifi();
  client.setServer(MQTT_HOST, 1883);
  client.setCallback(callback);
  stepper.setMaxSpeed(MOTOR_SPEED);
  stepper.setAcceleration(MOTOR_ACCEL);
  Serial.println("Stepper initialized");
}

void loop() {
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
    }
  }
}
