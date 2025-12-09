#ifndef __HW_PLATFORM__
#define __HW_PLATFORM__

#include "config.h"
#include "devices/Button.h"
#include "devices/Led.h"
#include "devices/ServoMotor.h"
#include "devices/Pir.h"
#include "devices/Sonar.h"

class HWPlatform {

public:
  HWPlatform(); //Constructor
  void init();
  void test();

  //Getters
  Button* getButton();
  Led*  getLed1();
  Led*  getLed2();
  Led*  getLed3();
  ServoMotor* getMotor();
  Pir* getPirSensor();
  Sonar* getSonar();

private:
  Button* pButton; //Button for reset
  Led* pLed1; //LED1 indicator
  Led* pLed2; //LED2 indicator
  Led* pLed3; //LED3 indicator
  ServoMotor* pMotor; //Servo motor for opening/closing the hangar door
  Pir* pPirSensor; //PIR presence sensor
  Sonar* pSonar; //Ultrasonic distance sensor
  //Manca il sensore di temperatura : quale ??

};

#endif
