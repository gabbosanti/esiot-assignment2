#include "HWPlatform.h"
#include <Arduino.h>
#include "devices/ButtonImpl.h"
#include "kernel/MsgService.h"
#include "config.h"
#include "devices/Led.h"
#include "devices/LightSensorImpl.h"
#include "devices/ServoMotorImpl.h"
#include "kernel/Logger.h"

void wakeUp(){}

HWPlatform::HWPlatform(){
  pButton = new ButtonImpl(RESET_BUTTON_PIN);
  pLed1 = new Led(L1_PIN);
  pLed2 = new Led(L2_PIN);
  pLed3 = new Led(L3_PIN);
  pMotor = new ServoMotorImpl(HD_SERVO_PIN);
  pPirSensor = new Pir(DPD_PIR_PIN);
  pSonar = new Sonar(DDD_ECHO_PIN, DDD_TRIG_PIN, 30000);
  pTempSensor = new TempSensorTMP36(TEMP_SENSOR_PIN);
  pLcd = new Lcd(0x27, 16, 2); 
}

void HWPlatform::init(){
}

//Getters

Button* HWPlatform::getButton(){
  return this->pButton;
}

Led* HWPlatform::getLed1(){
  return this->pLed1;
}

Led* HWPlatform::getLed2(){
  return this->pLed2;
}

Led* HWPlatform::getLed3(){
  return this->pLed3;
}

Pir* HWPlatform::getPirSensor(){
  return this->pPirSensor;
}

Sonar* HWPlatform::getSonar(){
  return this->pSonar;
}

ServoMotor* HWPlatform::getMotor(){
  return this->pMotor;
}

Lcd* HWPlatform::getLcd(){
  return this->pLcd;
} 

TempSensorTMP36* HWPlatform::getTempSensor(){
  return this->pTempSensor;
}

//Testing HW components (da fare meglio)
void HWPlatform::test(){
  bool btPressed = pButton->isPressed();
  pLed1->switchOn();
  pMotor->on();
  pMotor->setPosition(90);
  Logger.log("Button: " + String(btPressed ? "pressed" : " not pressed"));
  delay(1000);
  pMotor->setPosition(0);
  delay(1000);
  pMotor->off();
  pLed1->switchOff();
}

