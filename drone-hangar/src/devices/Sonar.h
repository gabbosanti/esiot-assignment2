#ifndef __SONAR__
#define __SONAR__

#include "ProximitySensor.h"

#define NO_OBJ_DETECTED -1

class Sonar : public ProximitySensor
{

public:
  Sonar(int echoPin, int trigPin, long maxTime);
  float getDistance();
  void setTemperature(float temp);

private:
  float getSoundSpeed();

  int echoPin, trigPin;
  long timeOut;
  float temperature;
};

#endif