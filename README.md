Handle state and transitions of Arduino projects.

The library
* helps to define the relevant states of an application
* allows to organize the code relevant to specific states by registering methods to call on entering, holding and exiting a specific state
* can register timeout handlers to limit time in a given state
* issues state transition debug messages on Serial
* leaves the decision of state transition to the registered handlers (no restrictions on possible transitions)

## Coding examples
### Definition

```
typedef enum                {PARK,   START,   MOVING,   STOPPED,   CHANGE } States;
const char* stateNames[] = {"Park", "Start", "Moving", "Stopped", "Change"};
StateMachine state(5, stateNames, millis);
```

The enum definitions ```States``` are later used to identify the states. 
The ```state``` machine will be initiated with the number of possible states.
The ```stateNames``` will be used for debug output. 
```millis``` is the default function to define the time in milliseconds (a different method could be given for testing or to offset sleep periods).

### Registration
```
void setup() {
  state.onEnter(START, onEnterStart);
  state.onState(START, onStart);
  state.onTimeout(START, 10 * 60 * 1000, onTimeoutStart); // time in millis
  state.onExit(START, onExitStart);
  ...
  state.toState(START); // initial state
}
```

Registrations are optional.
For each state there can be one handler ```onEnter```, ```onState```, ```onTimeout``` and ```onExit```.
The initial state transformation must be set with ```toState``` 

### Keep it running
```
void loop() {
  state.loop(); // will call the handler code
}
```

### Handler code
```
void onTimeoutStart() {
  ...
  state.toState(PARK);
}
```

 
## Compile with arduino-cli
```
arduino-cli compile examples/Platform -b arduino:avr:mega
```