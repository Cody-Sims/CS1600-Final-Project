#include <Servo.h>
#include "final_project_1600.h"

// Light and Sound Sensor Variables
const int lightThreshold = 500;
const int bufferSize =  30;
const int clapThreshold = 700;
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

State currentState = VERIFY_LIGHT_STATUS;

void setup() {
  Serial.begin(9600);
  delay(1000);
  initializeComponents();
  initializeWDT();
  initializeWifi();
  connectToNTP();
  printWiFiStatus();
}

void loop() {
  WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
  
  handleWiFiClient();
  int lightAmt = analogRead(photoresistorPin);
  state_inputs inputs = updateInputs();
  currentState = updateFSM(currentState, inputs);
}

state_inputs updateInputs() {
  state_inputs newInputs;
  newInputs.light_amt = analogRead(photoresistorPin);
  updateMicReading(analogRead(micPin));
  newInputs.claps = twoClaps();
  return newInputs;
}


State updateFSM(State currentState, state_inputs inputs) {
  State nextState = currentState;
  switch (currentState) {
    case VERIFY_LIGHT_STATUS:
      if (inputs.light_amt >= lightThreshold) {
        goalLightsOn = true;
        nextState = LIGHTS_ON_WAIT;
        Serial.println("Transition to LIGHTS_ON_WAIT");
      } else if (inputs.light_amt < lightThreshold) {
        goalLightsOn = false;
        nextState = LIGHTS_OFF_WAIT;
        Serial.println("Transition to LIGHTS_OFF_WAIT");
      } else {
        nextState = PRESS_BUTTON;
        Serial.println("Unsuccessful pushing button, transition to PRESS_BUTTON");
      }
      break;
    case LIGHTS_OFF_WAIT:
      if (inputs.light_amt > lightThreshold) {
        nextState = LIGHTS_ON_WAIT;
        goalLightsOn = true;
        Serial.println("Transition to LIGHTS_ON_WAIT, someone turned on the lights");
      } else if (inputs.claps) {
        goalLightsOn = true;
        nextState = PRESS_BUTTON;
        Serial.println("Clap detected, toggling lights.");
      } else if (timeStringToMinutes(getCurrentTime()) == timeStringToMinutes(wakeup_time)) {
        goalLightsOn = true;
        nextState = PRESS_BUTTON;
        Serial.println("Wakeup time, toggling lights.");
      } else {
        nextState = LIGHTS_OFF_WAIT;
      }
      break;
    case LIGHTS_ON_WAIT:
      if (inputs.light_amt < lightThreshold) {
        nextState = PRESS_BUTTON;
        goalLightsOn = true;
        Serial.println("Transition to LIGHTS_ON_WAIT, lights turned off");
      } else if (inputs.claps) {
        nextState = PRESS_BUTTON;
        goalLightsOn = false;
        Serial.println("Clap detected, toggling lights.");
      } else if (timeStringToMinutes(getCurrentTime()) == timeStringToMinutes(sleep_time)) {
        goalLightsOn = false;
        nextState = PRESS_BUTTON;
        Serial.println("Sleep time, toggling lights.");
      } else {
        nextState = LIGHTS_ON_WAIT;
      }
      break;
  case PRESS_BUTTON:
    PressSwitch();
    nextState = VERIFY_LIGHT_STATUS;
    break;
  }
  return nextState;
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
  // if (decibel > clapThreshold) {
  //   Serial.println(decibel);
  // }
}

// Convert a time string to minutes since midnight
int timeStringToMinutes(String timeStr) {
  int hour = timeStr.substring(0, 2).toInt();
  int minute = timeStr.substring(3, 5).toInt();
  return hour * 60 + minute;
}

// bool shouldTurnOnLights(int lightAmt) {
//     int currentTimeInMinutes = timeStringToMinutes(getCurrentTime());
//     int wakeupTimeInMinutes = timeStringToMinutes(wakeup_time);

//     if (currentTimeInMinutes == wakeupTimeInMinutes) {
//         Serial.println(currentTimeInMinutes);
//         Serial.println(wakeupTimeInMinutes);
//         Serial.println("Turning on lights because it's wakeup time.");
//         return true;
//     } else if (goalLightsOn && (lightAmt < lightThreshold)) {
//         Serial.println("Turning on lights because it's dark and goal is to have lights on.");
//         return true;
//     }
//     return false;
// }

// bool shouldTurnOffLights(int lightAmt) {
//     int currentTimeInMinutes = timeStringToMinutes(getCurrentTime());
//     int sleepTimeInMinutes = timeStringToMinutes(sleep_time);

//     if (currentTimeInMinutes == sleepTimeInMinutes) {
//         Serial.println(currentTimeInMinutes);
//         Serial.println(sleepTimeInMinutes);
//         Serial.println("Turning off lights because it's sleep time.");
//         return true;
//     } else if (!goalLightsOn && (lightAmt > lightThreshold)) {
//         Serial.println("Turning off lights because it's bright enough and goal is to have lights off.");
//         return true;
//     }
//     return false;
// }


bool twoClaps() {
  int clapCount = 0;
  for (int i = 0; i < bufferSize - 1; i++) {
    if (micReadings[i] > clapThreshold) {
      clapCount++;
    }
  }
  // if (clapCount == 1) {
  //   Serial.println("one claps detected.");
  // }
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

void initializeWDT() {
  // Initialize WDT
  NVIC_DisableIRQ(WDT_IRQn);
  NVIC_ClearPendingIRQ(WDT_IRQn);
  NVIC_SetPriority(WDT_IRQn, 0);
  NVIC_EnableIRQ(WDT_IRQn);

  // Configure and enable WDT GCLK
  GCLK->GENDIV.reg = GCLK_GENDIV_DIV(4) | GCLK_GENDIV_ID(5);
  while (GCLK->STATUS.bit.SYNCBUSY);

  GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN | 
                      GCLK_GENCTRL_DIVSEL | 
                      GCLK_GENCTRL_SRC(3) | 
                      GCLK_GENCTRL_ID(5);
  while(GCLK->STATUS.bit.SYNCBUSY);

  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | 
                      GCLK_CLKCTRL_GEN(5) | 
                      GCLK_CLKCTRL_ID(3);

  // Configure and enable WDT
  WDT->CONFIG.reg = WDT_CONFIG_PER(11);
  WDT->EWCTRL.reg = WDT_EWCTRL_EWOFFSET(10);
  WDT->CTRL.reg = WDT_CTRL_ENABLE;
  WDT->INTENSET.reg = WDT_INTENSET_EW; // Enable early warning interrupt
}

// WDT interrupt service routine
void WDT_Handler() {
  // Clear interrupt register flag
  WDT->INTFLAG.reg = WDT_INTFLAG_EW;
  Serial.println("Watchdog timer interrupt triggered!");
}


