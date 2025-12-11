#ifndef __SWEEPING_TASK__
#define __SWEEPING_TASK__

#include "kernel/Task.h"
#include "devices/ServoMotor.h"
#include "devices/Button.h"
#include "devices/Pir.h"
#include "devices/Sonar.h"
#include "devices/TempSensorTMP36.h"
#include "devices/Lcd.h"
#include "model/Context.h"
#include "Arduino.h"

class ControlHangarTask: public Task {

public:
  ControlHangarTask(Button* pButton, ServoMotor* pMotor, Sonar* pSonar, Pir* pPir, TempSensorTMP36* pTempSensor, Lcd* pLcd, Context* pContext); 
  void tick();

private:  
  void setState(int state);
  long elapsedTimeInState();
  void log(const String& msg);
  bool checkAndSetJustEntered();
    
  enum { IDLE, TAKEOFF, DRONE_OUT, LANDING, PRE_ALARM, ALARM } state;
  long stateTimestamp;
  bool justEntered;

  Button* pButton;
  ServoMotor* pMotor;
  Sonar* pSonar;
  Pir* pPir;
  TempSensorTMP36* pTempSensor;
  Lcd* pLcd;
  Context* pContext;

  bool pendingPreAlarm;
};

#endif