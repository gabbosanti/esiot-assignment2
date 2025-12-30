#ifndef __CONTROL_HANGAR_TASK__
#define __CONTROL_HANGAR_TASK__

#include "kernel/Task.h"
#include "devices/ServoMotor.h"
#include "devices/Button.h"
#include "devices/Pir.h"
#include "devices/Sonar.h"
#include "devices/TempSensorTMP36.h"
#include "devices/Lcd.h"
#include "model/Context.h"
#include "Arduino.h"

class ControlHangarTask : public Task
{

public:
  ControlHangarTask(Button *pButton, ServoMotor *pMotor, Sonar *pSonar, Pir *pPir, TempSensorTMP36 *pTempSensor, Lcd *pLcd, Context *pContext);
  void tick();
  unsigned int checkTemp(unsigned int ID, float TEMP);
  unsigned int checkDist(unsigned int ID, float DIST, char OPERATOR);

private:
  Button *pButton;
  ServoMotor *pMotor;
  Sonar *pSonar;
  Pir *pPir;
  TempSensorTMP36 *pTempSensor;
  Lcd *pLcd;
  Context *pContext;

  enum State
  {
    IDLE,
    TAKEOFF,
    DRONE_OUT,
    LANDING,
    PRE_ALARM,
    ALARM
  };
  State state;
  long stateTimestamp;
  bool justEntered;
  String pendingCmd = "";
  bool droneOutside = false;

  // TEMPERATURE (bool : stato della condizione, unsigned int: timestamp in cui è stata soddisfatta la condizione)
  bool temp1Cond = false;
  unsigned int temp1Start = 0;
  bool temp2Cond = false;
  unsigned int temp2Start = 0;

  // DISTANCE (bool : stato della condizione, unsigned int: timestamp in cui è stata soddisfatta la condizione)
  bool d1Cond = false;
  unsigned int d1Start = 0;
  bool d2Cond = false;
  unsigned int d2Start = 0;

  void setState(State state);
  long elapsedTimeInState();
  void log(const String &msg);
  bool checkAndSetJustEntered();
  void readSerial(bool allowed);
  void resetConditions();
};

#endif