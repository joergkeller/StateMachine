/**********************************************************
 * Example code demonstrating the use of a state machine
 * by showing the different states of a lifting platform.
 **********************************************************/

#include </Development/StateMachine/src/StateMachine.h>
#include "PlatformControl.h"

// Definition
typedef enum                {PARKED,   GO_PARK,   START,   MANUAL,   STOPPED,  GO_TOOL,    TOOLING} States;
const char* stateNames[] = {"Parked", "Go park", "Start", "Manual", "Stopped", "Go tool", "Tooling"};
StateMachine state(7, stateNames, millis);

PlatformControl platform;

void setup() {
  // Registration
  state.onEnter(START, enterStart);
  state.onState(START, onStart);
  state.onState(MANUAL, onManual);
  state.onState(STOPPED, onStopped);
  state.onState(GO_TOOL, onGoingTool);

  // Startup procedure to find start position
  if (!isParkPosition()) {
    platform.doParkBlocking();   // find lower bound
  }
  platform.doUnparkBlocking(); // go to start position
  state.toState(START);

  // TODO: Setup ISR
}

void loop() {
  // Run state handlers
  state.loop();
}

void enterStart() {
  platform.setStartPosition();
}

void onStart() {
  if (isUpButtonPressed() || isUpFastButtonPressed()) {
    state.toState(MANUAL);
  } else (isToolingButtonPressed()) {
    state.toState(GO_TOOL);
  } else (isParkButtonPressed()) {
    state.toState(GO_PARK);
  } else (isZeroButtonPressed()) {
    platform.setStartPosition();
  }
}

void onManual() {
  if (isParkPosition()) {
    platform.doUnparkBlocking();    // go to start again
    state.toState(START);
  } else if (isToolPosition()) {
    state.toState(TOOLING);         // cannot go further up
  } else if (isUpButtonPressed()) {
    platform.directionUp();
    platform.setSpeedNormal();
    platform.doMove();
  } else if (isUpFastButtonPressed()) {
    platform.directionUp();
    platform.setSpeedFast();
    platform.doMove();
  } else if (isDownButtonPressed()) {
    platform.directionDown();
    platform.setSpeedNormal();
    platform.doMove();
  } else if (isDownFastButtonPressed()) {
    platform.directionDown();
    platform.setSpeedFast();
    platform.doMove();
  } else if (isToolingButtonPressed()) {
    state.toState(GO_TOOL);
  } else if (isParkButtonPressed()) {
    state.toState(GO_PARK);
  } else {
    state.toState(STOPPED);
  }
}

void onStopped() {
  if (isUpButtonPressed() || isUpFastButtonPressed() || isDownButtonPressed() || isDownFastButtonPressed()) {
    state.toState(MANUAL);
  } else (isToolingButtonPressed()) {
    state.toState(GO_TOOL);
  } else (isParkButtonPressed()) {
    state.toState(GO_PARK);
  } else (isZeroButtonPressed()) {
    platform.setStartPosition();
  }
}

void onGoingTool() {
}

// ISR
void onParkSwitch() { platform.stop(); state.toState(PARKED); }
void onToolSwitch() { platform.stop(); state.toState(TOOLING); }

bool isParkPosition() {}
bool isToolPosition() {}
bool isParkButtonPressed() {}
bool isToolingButtonPressed() {}
bool isUpButtonPressed() {}
bool isUpFastButtonPressed() {}
bool isDownButtonPressed() {}
bool isDownFastButtonPressed() {}
