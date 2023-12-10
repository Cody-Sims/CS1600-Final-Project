

// /*
//  * A struct to keep all 9 state variables in one place
//  */
// typedef struct {
//   byte x;
//   byte y;
//   orientation o;
//   int lxb;
//   int uxb;
//   int level;
//   int timeStep;
//   int savedClock;
//   int countdown;
// } state_vars;

// bool testTransition(state startState,
//                      state endState,
//                      state_inputs testStateInputs, 
//                      state_vars startStateVars,
//                      state_vars endStateVars,
//                      bool verbos);
// /*        
//  * Helper function for printing states
//  */
// char* s2str(state s) {
//   switch(s) {
//     case sDISP_COUNTDOWN:
//     return "(1) DISP_COUNTDOWN";
//     case sWAIT_AFTER_ROT:
//     return "(2) WAIT_AFTER_ROT";
//     case sMOV:
//     return "(3) MOV";
//     case sWAIT_AFTER_MOV:
//     return "(4) WAIT_AFTER_MOV";
//     case sROT:
//     return "(5) ROT";
//     case sWAIT_FOR_BUT:
//     return "(6) WAIT_FOR_BUT";
//     case sGAME_OVER:
//     return "(7) GAME_OVER";
//     default:
//     return "???";
//   }
// }

// /*
//  * Helper function for printing orientations
//  */
// char* o2str(orientation o) {
//   switch(o) {
//     case UP:
//     return "UP";
//     case RIGHT:
//     return "RIGHT";
//     case DOWN:
//     return "DOWN";
//     case LEFT:
//     return "LEFT";
//     default:
//     return "???";
//   }
// }

// /*
//  * Given a start state, inputs, and starting values for state variables, tests that
//  * updateFSM returns the correct end state and updates the state variables correctly
//  * returns true if this is the case (test passed) and false otherwise (test failed)
//  * 
//  * Need to use "verbos" instead of "verbose" because verbose is apparently a keyword
//  */
// bool testTransition(state startState,
//                      state endState,
//                      state_inputs testStateInputs, 
//                      state_vars startStateVars,
//                      state_vars endStateVars,
//                      bool verbos) {
//   x = startStateVars.x;
//   y = startStateVars.y;
//   o = startStateVars.o;
//   lxb = startStateVars.lxb;
//   uxb = startStateVars.uxb;
//   level = startStateVars.level;
//   timeStep = startStateVars.timeStep;
//   savedClock = startStateVars.savedClock;
//   countdown = startStateVars.countdown;
//   state resultState = updateFSM(startState, testStateInputs.mils, testStateInputs.numButtons, testStateInputs.lastButton);
//   bool passedTest = (endState == resultState and
//                       x == endStateVars.x and
//                       y == endStateVars.y and
//                       o == endStateVars.o and
//                       lxb == endStateVars.lxb and
//                       uxb == endStateVars.uxb and
//                       level == endStateVars.level and
//                       timeStep == endStateVars.timeStep and
//                       savedClock == endStateVars.savedClock and
//                       countdown == endStateVars.countdown);
//   if (! verbos) {
//     return passedTest;
//   } else if (passedTest) {
//     char sToPrint[200];
//     sprintf(sToPrint, "Test from %s to %s PASSED", s2str(startState), s2str(endState));
//     Serial.println(sToPrint);
//     return true;
//   } else {
//     char sToPrint[200];
//     Serial.println(s2str(startState));
//     sprintf(sToPrint, "Test from %s to %s FAILED", s2str(startState), s2str(endState));
//     Serial.println(sToPrint);
//     sprintf(sToPrint, "End state expected: %s | actual: %s", s2str(endState), s2str(resultState));
//     Serial.println(sToPrint);
//     sprintf(sToPrint, "Inputs: mils %ld | numButtons %d | lastButton %s", testStateInputs.mils, testStateInputs.numButtons, o2str(testStateInputs.lastButton));
//     Serial.println(sToPrint);
//     sprintf(sToPrint, "          %2s | %2s | %5s | %3s | %3s | %5s | %9s | %11s | %9s", "x", "y", "o", "lxb", "uxb", "level", "timeStep", "savedClock", "countdown");
//     Serial.println(sToPrint);
//     sprintf(sToPrint, "expected: %2d | %2d | %5s | %3d | %3d | %5d | %9d | %11d | %9d", endStateVars.x, endStateVars.y, o2str(endStateVars.o), endStateVars.lxb, endStateVars.uxb, endStateVars.level, endStateVars.timeStep, endStateVars.savedClock, endStateVars.countdown);
//     Serial.println(sToPrint);
//     sprintf(sToPrint, "actual:   %2d | %2d | %5s | %3d | %3d | %5d | %9d | %11d | %9d", x, y, o2str(o), lxb, uxb, level, timeStep, savedClock, countdown);
//     Serial.println(sToPrint);
//     return false;
//   }
// }

// /*
//  * REPLACE THE FOLLOWING 6 LINES WITH YOUR TEST CASES
//  */
// const state testStatesIn[0] = {};
// const state testStatesOut[0] = {};
// const state_inputs testInputs[0] = {};
// const state_vars testVarsIn[0] = {};
// const state_vars testVarsOut[0] = {};
// const int numTests = 0;

// /*
//  * Runs through all the test cases defined above
//  */
// bool testAllTests() {
//   for (int i = 0; i < numTests; i++) {
//     Serial.print("Running test ");
//     Serial.println(i);
//     if (!testTransition(testStatesIn[i], testStatesOut[i], testInputs[i], testVarsIn[i], testVarsOut[i], true)) {
//       return false;
//     }
//     Serial.println();
//   }
//   Serial.println("All tests passed!");
//   return true;
// }