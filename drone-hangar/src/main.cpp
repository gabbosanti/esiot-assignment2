#include <Arduino.h>
#include "config.h"
#include "kernel/Scheduler.h"
#include "kernel/Logger.h"
#include "kernel/MsgService.h"
#include "model/HWPlatform.h"
#include "tasks/TestHWTask.h"
#include "tasks/ControlHangarTask.h"
#include "tasks/BlinkingTask.h"

#define __TESTING_HW__

Scheduler sched;
HWPlatform *pHWPlatform;
Context *pContext;

void setup()
{

  MsgService.init();
  sched.init(100);

  Logger.log(":::::: Smart Drone Hangar ::::");

  pHWPlatform = new HWPlatform();
  pHWPlatform->init();

#ifdef __TESTING_HW__
  pContext = new Context();

  Task *pControlHangarTask = new ControlHangarTask(pHWPlatform->getButton(), pHWPlatform->getMotor(), pHWPlatform->getSonar(), pHWPlatform->getPirSensor(), pHWPlatform->getTempSensor(), pHWPlatform->getLcd(), pContext);
  pControlHangarTask->init(100);

  Task *pBlinkingTask = new BlinkingTask(pHWPlatform->getLed1(), pHWPlatform->getLed2(), pHWPlatform->getLed3(), pContext);
  pBlinkingTask->init(100);

  sched.addTask(pControlHangarTask);
  sched.addTask(pBlinkingTask);
#endif

#ifndef __TESTING_HW__
  // Testing
  Task *pTestHWTask = new TestHWTask(pHWPlatform);
  pTestHWTask->init(50);
  sched.addTask(pTestHWTask);
#endif
}

void loop()
{
  sched.schedule();
}

// Test tmp36
/*
void setup()
{
  Serial.begin(115200);
  Serial.println("=== TMP36 Diagnostic ===");
}

void loop()
{
  int raw = analogRead(A0);
  float voltage = raw * 5.0 / 1024.0;
  float temp = (voltage - 0.5) * 100.0;

  Serial.print("Raw ADC: ");
  Serial.print(raw);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 3); // 3 decimali
  Serial.print("V | Temp: ");
  Serial.print(temp, 1);
  Serial.println("°C");

  delay(500);
}
*/