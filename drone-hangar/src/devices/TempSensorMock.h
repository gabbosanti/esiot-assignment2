#ifndef __TEMP_SENSOR_MOCK__
#define __TEMP_SENSOR_MOCK__

#include "TempSensor.h"

class TempSensorMock : public TempSensor {
public:
    TempSensorMock() : value(25.0f) {}
    void setTemperature(float v) { value = v; }
    float getTemperature() override { return value; }
private:
    float value;
};

#endif
