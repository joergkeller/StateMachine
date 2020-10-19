/**********************************************************
 * Example code demonstrating the use of a state machine
 * by showing the different states of a lifting platform.
 **********************************************************/

#include </Development/StateMachine/src/StateMachine.h>
#include "PlatformControl.h"

#define MS 1L
#define SEC  (1000*MS)
#define MIN  (60*SEC)

#define PARK_SWITCH_PIN  2
#define TOOL_SWITCH_PIN  3
#define PARK_BUTTON_PIN 18
#define TOOL_BUTTON_PIN 19
#define ZERO_BUTTON_PIN 20

// Definition
typedef enum                {PARKED,   GO_PARK,   START,   MANUAL,   STOPPED,  GO_TOOL,    TOOLING} States;
const char* stateNames[] = {"Parked", "Go park", "Start", "Manual", "Stopped", "Go tool", "Tooling"};
StateMachine state(7, stateNames, millis);

PlatformControl platform;
long            displayZero;  // position that should be displayed as 0.0 mm

void setup() {
  // Registration
  state.onEnter(START, onEnterStart);
  state.onEnter(STOPPED, onEnterStopped);
  state.onEnter(GO_TOOL, onEnterGoingTool);
  state.onState(GO_TOOL, onGoingTool);
  state.onTimeout(GO_TOOL, 1*MIN, onTimeoutGoingTool);
  state.onEnter(TOOLING, onEnterTooling);
  state.onEnter(GO_PARK, onEnterGoingPark);
  state.onState(GO_PARK, onGoingPark);
  state.onTimeout(GO_PARK, 2*MIN, onTimeoutGoingPark);
  state.onEnter(PARKED, onEnterParked);

  // Startup procedure to find start position
  if (!isParkPosition()) {
    platform.doParkBlocking();   // find lower bound
  }
  platform.doUnparkBlocking(); // go to start position
  state.toState(START);

  attachInterrupt(digitalPinToInterrupt(PARK_SWITCH_PIN), onParkSwitch, LOW);
  attachInterrupt(digitalPinToInterrupt(TOOL_SWITCH_PIN), onToolSwitch, LOW);
  attachInterrupt(digitalPinToInterrupt(PARK_BUTTON_PIN), onParkButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(TOOL_BUTTON_PIN), onToolButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(ZERO_BUTTON_PIN), onZeroButton, FALLING);
  // TODO: Setup ISR for up/down buttons 4x (pin change library)
}

void loop() {
  // Run state handlers
  state.loop();
}

void onEnterStart() {
  platform.zeroPosition();
  displayZero = 0L;
}

void onEnterStopped() {
  platform.stop();
}

void onEnterGoingTool() {
  platform.moveTop(FAST); // approx. position known
}

void onGoingTool() {
  if (!platform.moving()) {
    // moved up to approx. position but tool switch not contacted
    platform.moveUp(SLOW);
  }
}

void onTimeoutGoingTool() {
  // showAlarm()
  state.toState(STOPPED);
}

void onEnterTooling() {
  // showTooling()
}

void onEnterGoingPark() {
  platform.movePark(FAST); // approx. position known
}

void onGoingPark() {
  if (!platform.moving()) {
    // moved down to approx. position but tool switch not contacted
    platform.moveDown(SLOW);
  }
}

void onTimeoutGoingPark() {
  // showAlarm()
  state.toState(STOPPED);
}

void onEnterParked() {
  // showParked()
}

// ISR
void onParkSwitch() { platform.stop(); state.toState(PARKED); }
void onToolSwitch() { platform.stop(); state.toState(TOOLING); }

void onParkButton() { state.toState(GO_PARK); }
void onToolButton() { state.toState(GO_TOOL); }
void onZeroButton() { displayZero = platform.position(); }

void onUpButton() {
  if (state.state() == TOOLING) return;
  if (isUpButtonPressed()) {
    platform.moveUp(SLOW);
    state.toState(MANUAL);
  } else {
    state.toState(STOPPED);
  }
}

void onUpFastButton() {
  if (state.state() == TOOLING) return;
  if (isUpFastButtonPressed()) {
    platform.moveUp(FAST);
    state.toState(MANUAL);
  } else {
    state.toState(STOPPED);
  }
}

void onDownButton() {
  if (state.state() == PARKED) return;
  if (state.state() == START) return;
  if (isDownButtonPressed()) {
    platform.moveDown(SLOW);
    state.toState(MANUAL);
  } else {
    state.toState(STOPPED);
  }
}

void onDownFastButton() {
  if (state.state() == PARKED) return;
  if (state.state() == START) return;
  if (isDownButtonPressed()) {
    platform.moveDown(FAST);
    state.toState(MANUAL);
  } else {
    state.toState(STOPPED);
  }
}

bool isParkPosition() {}
bool isToolPosition() {}

bool isParkButtonPressed() {}
bool isToolButtonPressed() {}
bool isUpButtonPressed() {}
bool isUpFastButtonPressed() {}
bool isDownButtonPressed() {}
bool isDownFastButtonPressed() {}
