#include "final_project_1600.h"

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
  }

  Serial.println("Time tests passed");

  Serial.println("All tests passed!");
  return true;
}