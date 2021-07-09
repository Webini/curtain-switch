#ifndef MS5611_SENSOR_H
#define MS5611_SENSOR_H

#include <Wire.h>
#include <MS5x.h>
#include "AbstractSensor.h"
#include "log.h"

#define DEFAULT_SENSOR_SDA_PIN 4 
#define DEFAULT_SENSOR_SCL_PIN 5
#define DEFAULT_SENSOR_ADDR    0x77
#define DEFAULT_SENSOR_DELAY   1000
#define SEALEVELPRESSURE_HPA   1013.25

class MS5611Sensor : public AbstractSensor {
  public:
    MS5611Sensor(int sensorSdaPin = DEFAULT_SENSOR_SDA_PIN, int sensorSclPin = DEFAULT_SENSOR_SCL_PIN, int sensorAddr = DEFAULT_SENSOR_ADDR, int sensorDelay = DEFAULT_SENSOR_DELAY);
    MS5611Sensor();
    void begin();
    void loop();
    double getTemperature();
    double getPressure();
    double getAltitude();

  private:
    byte sensorSdaPin = DEFAULT_SENSOR_SDA_PIN;
    byte sensorSclPin = DEFAULT_SENSOR_SCL_PIN;
    byte sensorAddr = DEFAULT_SENSOR_ADDR;
    int sensorDelay = DEFAULT_SENSOR_DELAY;
    unsigned long lastUpdatedAt = 0;
    TwoWire wire;
    MS5x barometer;
};


#endif
