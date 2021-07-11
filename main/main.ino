#include <EEPROM.h>
#include "parameters.h"
#include "log.h"
#include "GlobalConfiguration.h"
#include "Commander.h"
#include "HardManager.h"
#include "SleepMonitor.h"
#include "MS5611Sensor.h"

MS5611Sensor sensor(SENSOR_SDA_PIN, SENSOR_SCL_PIN, SENSOR_I2C_ADDRS, SENSOR_DELAY);
SleepMonitor sleepMonitor;
GlobalConfiguration conf;
HardManager hardman;
Commander com(&conf, &hardman, &sleepMonitor, &sensor);

void setup() {
  Serial.begin(115200);
  Serial.print("Version : ");
  Serial.println(VERSION);

  sleepMonitor.begin();
  conf.begin();
  hardman.begin(conf.getDuration());  
  com.begin(conf.getMiddleCoursePosition());
  sensor.begin();
}

void loop() {
  sleepMonitor.startLoop();

  sensor.loop();
  hardman.loop();
  com.loop();

  sleepMonitor.endLoop();
}
