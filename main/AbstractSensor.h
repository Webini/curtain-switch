#ifndef ABSTRACT_SENSOR_H
#define ABSTRACT_SENSOR_H

class AbstractSensor {
  public:
    virtual void begin() = 0;
    virtual void loop() = 0;
    virtual bool isDetected() = 0;
    virtual byte getAddress() = 0;
    virtual double getTemperature() = 0;
    virtual double getPressure() = 0;
    virtual double getAltitude() = 0;
};

#endif
