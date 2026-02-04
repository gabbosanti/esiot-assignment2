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
  Led* pLed1;
  Led* pLed2;
  Led* pLed3;
  Context* pContext;

  enum State { IDLE, L2_ON, L2_OFF, PRE_ALARM, LED_ALARM };
  State state;
  void setState(State state);
  long elapsedTimeInState();
  bool checkAndSetJustEntered();
  
  long stateTimestamp;
  bool justEntered;

};

#endif