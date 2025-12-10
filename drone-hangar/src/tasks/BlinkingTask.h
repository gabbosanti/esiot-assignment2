#ifndef __BLINKING_TASK__
#define __BLINKING_TASK__

#include "kernel/Task.h"
#include "model/Context.h"
#include "devices/Led.h"
#include <Arduino.h>

class BlinkingTask: public Task {

public:
  BlinkingTask(Led* pLed1, Led* pLed2, Led* pLed3, Context* pContext); 
  void tick();

private:  
  void setState(int state);
  long elapsedTimeInState();
  
  bool checkAndSetJustEntered();
  
  enum { IDLE, TAKEOFF, DRONE_OUT, LANDING, PRE_ALARM, ALARM } state;
  long stateTimestamp;
  bool justEntered;

  Led* pLed1;
  Led* pLed2;
  Led* pLed3;
  Context* pContext;
};

#endif