#include "HWPlatform.h"
#include "Arduino.h"
#include "devices/Button.h"
#include "kernel/MsgService.h"
#include "config.h"
#include "devices/Led.h"
#include "devices/LightSensorImpl.h"
#include "devices/ServoMotorImpl.h"
#include "kernel/Logger.h"

void wakeUp() {}

HWPlatform::HWPlatform()
{
  pButton = new Button(RESET_BUTTON_PIN);
  pLed1 = new Led(L1_PIN);
  pLed2 = new Led(L2_PIN);
  pLed3 = new Led(L3_PIN);
  pMotor = new ServoMotorImpl(HD_SERVO_PIN);
  pPirSensor = new Pir(DPD_PIR_PIN);
  pSonar = new Sonar(DDD_ECHO_PIN, DDD_TRIG_PIN, 30000);
  pTempSensor = new TempSensorTMP36(TEMP_ANALOG_PIN);
  pLcd = new Lcd(0x27, 16, 2);
}

void HWPlatform::init()
{
}

// Getters

Button *HWPlatform::getButton()
{
  return this->pButton;
}

Led *HWPlatform::getLed1()
{
  return this->pLed1;
}

Led *HWPlatform::getLed2()
{
  return this->pLed2;
}

Led *HWPlatform::getLed3()
{
  return this->pLed3;
}

Pir *HWPlatform::getPirSensor()
{
  return this->pPirSensor;
}

Sonar *HWPlatform::getSonar()
{
  return this->pSonar;
}

ServoMotor *HWPlatform::getMotor()
{
  return this->pMotor;
}

Lcd *HWPlatform::getLcd()
{
  return this->pLcd;
}

TempSensorTMP36 *HWPlatform::getTempSensor()
{
  return this->pTempSensor;
}

// Funzioni di test per ogni componente :
void HWPlatform::testButton()
{
  static bool last = false;
  bool now = pButton->isPressedEdge();
  if (now != last)
  {
    Logger.log(String("Button: ") + (now ? "pressed" : "released"));
    last = now;
  }
}

void HWPlatform::testMotor()
{
  pMotor->on();
  pMotor->setPosition(90);
  delay(500);
  pMotor->setPosition(0);
  delay(500);
  pMotor->off();
}

void HWPlatform::testSonar()
{
  double d = pSonar->getDistance();
  Logger.log("Sonar distance: " + String(d));
}

void HWPlatform::testPir()
{
  Logger.log("Pir calibrating ...");
  pPirSensor->calibrate();
  Logger.log("Calibration ended");
  static bool last = false;
  pPirSensor->sync(); // aggiorna stato interno
  bool now = pPirSensor->isDetected();
  if (now != last)
  {
    Logger.log(now ? "Motion detected" : "No motion");
    last = now;
  }
}

void HWPlatform::testTemp()
{
  float t = pTempSensor->getTemperature();
  Logger.log("Temperature: " + String(t));
}

void HWPlatform::testLcd()
{
  pLcd->init();
  pLcd->print("PROVA");
}

// Testing dei componenti : uso la serial line per scegliere quale componente testare
void HWPlatform::test()
{

  if (Serial.available())
  {
    char c = Serial.read();
    switch (c)
    {
    case 'b':
      testButton();
      break;
    case 'm':
      testMotor();
      break;
    case 's':
      testSonar();
      break;
    case 'p':
      testPir();
      break;
    case 't':
      testTemp();
      break;
    case 'l':
      testLcd();
      break;
    }
  }
}
