#include "MS5611Sensor.h"

MS5611Sensor::MS5611Sensor(int _sensorSdaPin, int _sensorSclPin, int _sensorAddr, int _sensorDelay) 
  : sensorSdaPin(_sensorSdaPin), sensorSclPin(_sensorSclPin), sensorAddr(_sensorAddr), sensorDelay(_sensorDelay), barometer(&wire) {}

MS5611Sensor::MS5611Sensor() : barometer(&wire) {}

void MS5611Sensor::begin() {
  log_printf(
    "[MS5611Sensor::begin] Begin sensor sda pin: %d, scl pin: %d, addr: 0x%02X, delay: %d", 
    this->sensorSdaPin, 
    this->sensorSclPin,
    this->sensorAddr,
    this->sensorDelay
  );
  
  if (this->sensorSdaPin >= 0 && this->sensorSclPin >= 0) {
    this->wire.begin(this->sensorSdaPin, this->sensorSclPin);
  } else {
    this->wire.begin();
  }
  
  this->barometer.setI2Caddr(this->sensorAddr); 
  this->barometer.setSamples(MS5xxx_CMD_ADC_256);
  this->barometer.setDelay(this->sensorDelay);

  log_printf("[MS5611Sensor::begin] Connect result %d", this->barometer.connect());
}

void MS5611Sensor::loop() {
  unsigned long now = millis();
  if (now - this->lastUpdatedAt >= this->sensorDelay) {
    this->barometer.checkUpdates();
    this->lastUpdatedAt = now;
  }
}

double MS5611Sensor::getTemperature() {
  return this->barometer.GetTemp();
}

double MS5611Sensor::getPressure() {
  return this->barometer.GetPres() / 100.0;
}

double MS5611Sensor::getAltitude() {
  double pressure = this->getPressure();
  return 44330.0 * (1.0 - pow(pressure / SEALEVELPRESSURE_HPA, 0.19029495));
}
