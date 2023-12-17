#include "final_project_1600.h"

int button_pushes = 0;
bool testTransition(State startState,
                     State endState,
                     state_inputs testStateInputs, 
                     bool initial_goal,
                     bool final_goal,
                     bool verbos);
// /*        
//  * Helper function for printing states
//  */

char* s2str(State s) {
  switch(s) {
    case LIGHTS_OFF_WAIT:
      return "(2) LIGHTS_OFF_WAIT";
    case LIGHTS_ON_WAIT:
      return "(3) LIGHTS_ON_WAIT";
    case VERIFY_LIGHT_STATUS:
      return "(1) VERIFY_LIGHT_STATUS";
    case PRESS_BUTTON:
      return "(4) PRESS_BUTTON";
    default:
      return "???";
  }
}



// /*
//  * Given a start state, inputs, and starting values for state variables, tests that
//  * updateFSM returns the correct end state and updates the state variables correctly
//  * returns true if this is the case (test passed) and false otherwise (test failed)
//  * 
//  * Need to use "verbos" instead of "verbose" because verbose is apparently a keyword
//  */
bool testTransition(State startState,
                     State endState,
                     state_inputs testStateInputs, 
                     bool initial_goal,
                     bool final_goal,
                     bool verbos) {
  
  int light_amt = testStateInputs.light_amt;
  bool twoClaps = testStateInputs.claps;
  goalLightsOn = initial_goal;
  State resultState = updateFSM(startState, testStateInputs);
  bool passedTest = (endState == resultState and
                      goalLightsOn == final_goal);


  if (! verbos) {
    return passedTest;
  } else if (passedTest) {
    char sToPrint[200];
    sprintf(sToPrint, "Test from %s to %s PASSED", s2str(startState), s2str(endState));
    Serial.println(sToPrint);
    return true;
  } else {
    char sToPrint[200];
    Serial.println(s2str(startState));
    sprintf(sToPrint, "Test from %s to %s FAILED", s2str(startState), s2str(endState));
    Serial.println(sToPrint);
    sprintf(sToPrint, "End state expected: %s | actual: %s", s2str(endState), s2str(resultState));
    Serial.println(sToPrint);
    return false;
  }
}

// /*
//  * REPLACE THE FOLLOWING 6 LINES WITH YOUR TEST CASES
//  */
const State testStatesIn[11] = {(State) 0, (State) 0, (State) 0, (State) 0, (State) 1, (State) 1, (State) 1, (State) 2, (State) 2, (State) 2, (State) 3};

const State testStatesOut[11] = {(State) 1, (State) 2, (State) 3, (State) 3, (State) 1, (State) 2, (State) 3, (State) 3, (State) 3, (State) 2, (State) 0};

const state_inputs testInputs[11] = {{100, false}, {900, false}, {100, false}, {900, false}, {100, false}, {900, false}, {100, true}, {100, false}, {900, true}, {900, false}, {44, true}};

const bool testVarsIn[11] = {false, true, true, false, false, false, false, true, true, true, false};

const bool testVarsOut[11] = {false, true, true, false, false, true, true, true, false, true, false};

const int numTests = 11;


bool clapTest_0() {
  resetBuffer();
  for (int i=0; i < bufferSize; i++) {
    micReadings[i] = 0;
  }
  return !twoClaps();
}

bool clapTest_1() {
  resetBuffer();
  micReadings[0] = 1000;
  for (int i=1; i < bufferSize; i++) {
    micReadings[i] = 0;
  }
  return !twoClaps();
}

bool clapTest_2() {
  resetBuffer();
  micReadings[0] = 1000;
  for (int i=1; i < bufferSize - 1; i++) {
    micReadings[i] = 0;
  }
  micReadings[bufferSize - 1] = 1000;
  return twoClaps();
}

//After Dec 14 2024 this test should fail
bool ntpTimeTest() {
  unsigned long timeWhenWritingTest = 3911112622;
  unsigned long timeInAYear = 31536000;
  unsigned long currTime = getSecsSince1900();
  return (currTime > timeWhenWritingTest) && (currTime < timeWhenWritingTest + timeInAYear);
}

// Checks a series of state transitions
bool longTest() {
  State currentState = VERIFY_LIGHT_STATUS;
  goalLightsOn = false;
  state_inputs init_input = {0, false};

  // Initialize to light off wait
  currentState = updateFSM(currentState, init_input);
  if (currentState != LIGHTS_OFF_WAIT) {
    Serial.println("1");
    return false;
  }

  // If no inputs change, system stays the same
  for (int i = 0; i < 10; i++) {
    currentState = updateFSM(currentState, init_input);
  }

  if (currentState != LIGHTS_OFF_WAIT) {
    Serial.println("2");
    return false;
  }

  // User turns on light, system should recognize the user wants the lights on
  state_inputs input1 = {1000, false};

  for (int i = 0; i < 10; i++) {
    currentState = updateFSM(currentState, input1);
  }

  if (currentState != LIGHTS_ON_WAIT) {
    Serial.println("3");
    return false;
  }

  // User claps twice
  state_inputs input2 = {1000, true};
  
  for (int i = 0; i < 10; i++) {
    currentState = updateFSM(currentState, input2);
  }

  // System will attempt to push button, and verify the lights are turned off. While the polled value is greater than threshold, should continue to attempt to push button

  if (!((currentState == VERIFY_LIGHT_STATUS) || (currentState == PRESS_BUTTON))) {
    Serial.println("4");
    return false;
  }

  // Once the system polls the lights are off, returns to LIGHTS_OFF_WAIT
  state_inputs input3 = {0, false};

  for (int i = 0; i < 2; i++) {
    currentState = updateFSM(currentState, input3);
  }

  if (currentState != LIGHTS_OFF_WAIT) {
    Serial.println("5");
    return false;
  }

  return true;
}

/**
 * Use case scenario 1, testing resetting lights if they turn off automatically
 */
bool integrationTest1() {
  State currentState = LIGHTS_ON_WAIT;
  goalLightsOn = true;
  state_inputs init_input = {0, false};
  button_pushes = 0;

  // With an input in lights on wait where the light level is below the threshhold, push the button
  currentState = updateFSM(currentState, init_input);
  if (currentState != PRESS_BUTTON ) {
    Serial.println("1");
    return false;
  }


  state_inputs input1 = {1000, false};
  currentState = updateFSM(currentState, input1);
  if (currentState != VERIFY_LIGHT_STATUS || button_pushes != 1) {
    Serial.println("2");
    return false;
  }

  currentState = updateFSM(currentState, input1);
  if (currentState != LIGHTS_ON_WAIT) {
    Serial.println("3");
    return false;
  }

  return true;
}

/**
 * Use case scenario 2, testing microphone functionality
 */
bool integrationTest2() {
  State currentState = LIGHTS_ON_WAIT;
  goalLightsOn = true;
  button_pushes = 0;
  resetBuffer();

  updateMicReading(1000); // First clap - the argument is the scaled output of the microphone
  currentState = updateFSM(currentState, {1000, twoClaps()});
  if (currentState != LIGHTS_ON_WAIT) {
    Serial.println("1");
    return false;
  }

  updateMicReading(0); // Wait
  currentState = updateFSM(currentState, {1000, twoClaps()});
  if (currentState != LIGHTS_ON_WAIT) {
    Serial.println("2");
    return false;
  }


  updateMicReading(1000); // Second clap
  currentState = updateFSM(currentState, {1000, twoClaps()});
  if (currentState != PRESS_BUTTON) {
    Serial.println("3");
    return false;
  }

  updateMicReading(0); // Make sure the button is pushed once
  currentState = updateFSM(currentState, {1000, twoClaps()});
  if (currentState != VERIFY_LIGHT_STATUS || button_pushes != 1) {
    Serial.println("4");
    return false;
  }

  //Assume button press succeeded and light level now reads 0
  updateMicReading(0);
  currentState = updateFSM(currentState, {0, twoClaps()});
  if (currentState != LIGHTS_OFF_WAIT) {
    Serial.println("5");
    return false;
  }

  //System should remain in LIGHT_OFF_WAIT, twoClaps() should be returning false as it was already processed
  updateMicReading(0);
  currentState = updateFSM(currentState, {0, twoClaps()});
  if (currentState != LIGHTS_OFF_WAIT) {
    Serial.println("6");
    return false;
  }

  return true;
}

/**
 * Convert NTP time to a human-readable format using a specified NTP time
 */
String getCurrentTimeTest(unsigned long currentTime) {
  unsigned long epoch = currentTime - 2208988800UL;
  epoch -= 5 * 3600;
  unsigned long hour = (epoch % 86400L) / 3600;
  String hourStr = hour < 10 ? "0" + String(hour) : String(hour);
  unsigned long minute = (epoch % 3600) / 60;
  String minuteStr = minute < 10 ? "0" + String(minute) : String(minute);
  return hourStr + ":" + minuteStr;
}


/**
 * Use case scenario 3, testing wakeup functionality
 */
bool integrationTest3() {
  State currentState = LIGHTS_OFF_WAIT;
  goalLightsOn = true;
  button_pushes = 0;
  resetBuffer();

  // Set the wakeup time equal to the current time as requested from the NTP server, use the original time when the code was set up 
  // + millis() in FSM to show that the two are equivalent, causing a wake up to trigger

  unsigned long old_secs = getSecsSince1900();
  connectToNTP(); // side effect of setting secsSince1900 to the time received from the NTP server
  unsigned long new_secs = getSecsSince1900();
  setSecsSince1900(old_secs); // reset secsSince1900 to original value
  wakeup_time = getCurrentTimeTest(new_secs); // set the wakeup time to the current time

  currentState = updateFSM(currentState, {0, false});
  if (currentState != PRESS_BUTTON) {
    Serial.println("0");
    return false;
  }

  currentState = updateFSM(currentState, {0, false});
  if (currentState != VERIFY_LIGHT_STATUS || button_pushes != 1) {
    Serial.println("1");
    return false;
  }

  currentState = updateFSM(currentState, {1000, false});
  if (currentState != LIGHTS_ON_WAIT) {
    Serial.println("2");
    return false;
  }

  return true;
}

// /*
//  * Runs through all the test cases defined above
//  */
bool testAllTests() {
  for (int i = 0; i < numTests; i++) {
    Serial.print("Running test ");
    Serial.println(i);
    if (!testTransition(testStatesIn[i], testStatesOut[i], testInputs[i], testVarsIn[i], testVarsOut[i], true)) {
      return false;
    }
    Serial.println();
  }

  if (!clapTest_0()) {
    Serial.println("clapTest 0 failed");
    return false;
  }

  if (!clapTest_1()) {
    Serial.println("clapTest 1 failed");
    return false;
  }

  if (!clapTest_2()) {
    Serial.println("clapTest 2 failed");
    return false;
  }

  Serial.println("Clap tests passed");

  if (!ntpTimeTest()) {
    Serial.println("Current time test failed");
    return false;
  }

  Serial.println("Time tests passed");

  if (!longTest()) {
    Serial.println("Long test failed");
    return false;
  }

  if (!integrationTest1()) {
    Serial.println("Integration test 1 failed");
    return false;
  }

  if (!integrationTest2()) {
    Serial.println("Integration test 2 failed");
    return false;
  }

  if (!integrationTest3()) {
    Serial.println("Integration test 3 failed");
    return false;
  }

  Serial.println("Integration test passed!");

  Serial.println("All tests passed!");
  return true;
}