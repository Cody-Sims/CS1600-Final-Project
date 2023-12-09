#include <Servo.h>
#include "final_project_1600.h"

// Light and Sound Sensor Variables
const int lightThreshold = 500;
const int bufferSize = 10;
const int clapThreshold = 600;
const int photoresistorPin = A1;
const int micPin = A0;
const int servoPin = 6;
const int mosfetGatePin = 7;

// Mic Variables
int micReadings[bufferSize] = {0};
int idx = 0;
bool goalLightsOn = true;

// Servo
Servo myServo;

// State Management
enum State { LIGHTS_OFF_WAIT, LIGHTS_ON_WAIT };
State currentState = LIGHTS_ON_WAIT;

void setup() {
  Serial.begin(9600);
  delay(1000);
  initializeComponents();
  initializeWifi();
  connectToNTP();
  printWiFiStatus();
}

void loop() {
  handleWiFiClient();

  int lightAmt = analogRead(photoresistorPin);

  switch (currentState) {
    case LIGHTS_OFF_WAIT:
      if (shouldTurnOnLights(lightAmt)) {
        currentState = LIGHTS_ON_WAIT;
        Serial.println("Transition to LIGHTS_ON_WAIT");
        PressSwitch();
        goalLightsOn = true;
      } else if (twoClaps()) {
        goalLightsOn = !goalLightsOn;
        PressSwitch();
        Serial.println("Clap detected, toggling lights.");
      }
      break;

    case LIGHTS_ON_WAIT:
      if (shouldTurnOffLights(lightAmt)) {
        currentState = LIGHTS_OFF_WAIT;
        Serial.println("Transition to LIGHTS_OFF_WAIT");
        PressSwitch();
        goalLightsOn = false;
      } else if (twoClaps()) {
        goalLightsOn = !goalLightsOn;
        PressSwitch();
        Serial.println("Clap Detected, toggling lights.");
      }
      break;
  }

  delay(50);
}


void PressSwitch() {
  Serial.println("Pressing the switch...");
  digitalWrite(mosfetGatePin, HIGH); // Turn on MOSFET
  myServo.write(0);
  delay(500);
  myServo.write(180);
  delay(500);
  digitalWrite(mosfetGatePin, LOW); // Turn off MOSFET
  Serial.println("Switch pressed.");
}

void updateMicReading(int decibel) {
  micReadings[idx] = decibel;
  idx = (idx + 1) % bufferSize;
  Serial.print("Mic reading updated: ");
  Serial.println(decibel);
}

// Convert a time string to minutes since midnight
int timeStringToMinutes(String timeStr) {
  int hour = timeStr.substring(0, 2).toInt();
  int minute = timeStr.substring(3, 5).toInt();
  return hour * 60 + minute;
}

bool shouldTurnOnLights(int lightAmt) {
    int currentTimeInMinutes = timeStringToMinutes(getCurrentTime());
    int wakeupTimeInMinutes = timeStringToMinutes(wakeup_time);

    if (currentTimeInMinutes == wakeupTimeInMinutes) {
        Serial.println(currentTimeInMinutes);
        Serial.println(wakeupTimeInMinutes);
        Serial.println("Turning on lights because it's wakeup time.");
        return true;
    } else if (goalLightsOn && (lightAmt < lightThreshold)) {
        Serial.println("Turning on lights because it's dark and goal is to have lights on.");
        return true;
    }
    return false;
}

bool shouldTurnOffLights(int lightAmt) {
    int currentTimeInMinutes = timeStringToMinutes(getCurrentTime());
    int sleepTimeInMinutes = timeStringToMinutes(sleep_time);

    if (currentTimeInMinutes == sleepTimeInMinutes) {
        Serial.println(currentTimeInMinutes);
        Serial.println(sleepTimeInMinutes);
        Serial.println("Turning off lights because it's sleep time.");
        return true;
    } else if (!goalLightsOn && (lightAmt > lightThreshold)) {
        Serial.println("Turning off lights because it's bright enough and goal is to have lights off.");
        return true;
    }
    return false;
}


bool twoClaps() {
  int clapCount = 0;
  for (int i = 0; i < bufferSize - 1; i++) {
    if (micReadings[i] > clapThreshold && micReadings[i + 1] < clapThreshold) {
      clapCount++;
    }
  }
  if (clapCount >= 2) {
    resetBuffer();
    Serial.println("Two claps detected.");
    return true;
  }
  return false;
}

void resetBuffer() {
  for (int i = 0; i < bufferSize; i++) {
    micReadings[i] = 0;
  }
  idx = 0;
  Serial.println("Microphone buffer reset.");
}

void initializeComponents() {
  pinMode(photoresistorPin, INPUT);
  pinMode(micPin, INPUT);
  pinMode(mosfetGatePin, OUTPUT);
  digitalWrite(mosfetGatePin, LOW);
  myServo.attach(servoPin);
  myServo.write(0);
}

