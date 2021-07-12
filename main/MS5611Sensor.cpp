#include "MS5611Sensor.h"

MS5611Sensor::MS5611Sensor(int _sensorSdaPin, int _sensorSclPin, const byte* _sensorAddrs, int _sensorDelay) 
  : sensorSdaPin(_sensorSdaPin), sensorSclPin(_sensorSclPin), sensorAddrs(_sensorAddrs), sensorDelay(_sensorDelay), barometer(&wire) {}

MS5611Sensor::MS5611Sensor() : barometer(&wire) {}

void MS5611Sensor::begin() {
  log_printf(
    "[MS5611Sensor::begin] Begin sensor sda pin: %d, scl pin: %d, delay: %d", 
    this->sensorSdaPin, 
    this->sensorSclPin,
    this->sensorDelay
  );
  
  if (this->sensorSdaPin >= 0 && this->sensorSclPin >= 0) {
    this->wire.begin(this->sensorSdaPin, this->sensorSclPin);
  } else {
    this->wire.begin();
  }
  
  this->barometer.setDelay(this->sensorDelay);

  for (byte offset = 0; offset[this->sensorAddrs] != 0; offset++) { 
    this->barometer.setI2Caddr(this->sensorAddrs[offset]); 
    auto result = this->barometer.connect();
    log_printf("[MS5611Sensor::begin] Connect to 0x%02X result %d", this->sensorAddrs[offset], result);
    if (result == 0) {
      this->detected = true;
      this->failureDetectedAt = 0;
      this->foundAddr = this->sensorAddrs[offset];
      break;
    }
  }
}


byte MS5611Sensor::getAddress() {
  return this->foundAddr;
}


void MS5611Sensor::loop() {
  if (!this->detected) {
    return;
  }
  
  unsigned long now = millis();
  if (now - this->lastUpdatedAt >= this->sensorDelay) {
    this->barometer.checkUpdates();
    this->lastUpdatedAt = now;
    
    if (this->barometer.GetTemp() == 0 && this->failureDetectedAt == 0) {
      this->failureDetectedAt = now;
      log_printf("[MS5611Sensor::loop] Failure detected");
    }
  
    if (this->failureDetectedAt != 0 && now - this->failureDetectedAt > 5000) {
      this->barometer.connect();
      log_printf("[MS5611Sensor::loop] Reconnection to sensor");
      this->failureDetectedAt = 0;
    }
  }
}

bool MS5611Sensor::isDetected() {
  return this->detected;
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
