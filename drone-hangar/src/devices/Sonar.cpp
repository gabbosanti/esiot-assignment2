#include "Sonar.h"
#include "Arduino.h"

Sonar::Sonar(int echoP, int trigP, long maxTime)
    : echoPin(echoP), trigPin(trigP), timeOut(maxTime), temperature(20.0)
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void Sonar::setTemperature(float temp)
{
  temperature = temp;
}

float Sonar::getSoundSpeed()
{
  return 331.5 + 0.6 * temperature;
}

float Sonar::getDistance()
{
  // Assicurati che il trigger sia LOW
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Invia pulse di 10us
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Leggi il tempo di risposta
  float tUS = pulseIn(echoPin, HIGH, timeOut);

  if (tUS == 0)
  {
    return NO_OBJ_DETECTED;
  }
  else
  {
    float t = tUS / 1000.0 / 1000.0 / 2;
    float d = t * getSoundSpeed();
    return d;
  }
}