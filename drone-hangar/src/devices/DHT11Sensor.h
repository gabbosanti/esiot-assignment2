#ifndef __DHT11_SENSOR__
#define __DHT11_SENSOR__

#include "Arduino.h"
#include "TempSensor.h"

class DHT11Sensor : public TempSensor
{

public:
    DHT11Sensor(int pin);
    float getTemperature();
    float getHumidity();
    bool isValid();

private:
    int pin;
    float lastTemperature;
    float lastHumidity;
    bool lastReadValid;
    unsigned long lastReadTime;

    static const unsigned long MIN_READ_INTERVAL = 2000; // DHT11 richiede almeno 2 secondi tra letture

    bool readSensor();
    bool readByte(uint8_t &byte);
    bool waitForPulse(bool level, unsigned long timeout);
};

#endif