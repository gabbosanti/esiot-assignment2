#ifndef __BLINKING_TASK__
#define __BLINKING_TASK__

#include "kernel/Task.h"
#include "model/Context.h"
#include "devices/Led.h"
#include <Arduino.h>

class BlinkingTask: public Task {

public:
  BlinkingTask(Led* pLed, Context* pContext); 
  void tick();

private:  
  void setState(int state);
  long elapsedTimeInState();
  
  bool checkAndSetJustEntered();
  
  enum { IDLE, BLINK_OFF, BLINK_ON } state;
  long stateTimestamp;
  bool justEntered;

  Led* pLed;
  Context* pContext;
};

#endif