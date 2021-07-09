#include <EEPROM.h>
#include "parameters.h"
#include "log.h"
#include "GlobalConfiguration.h"
#include "Commander.h"
#include "HardManager.h"
#include "SleepMonitor.h"

#ifdef SENSOR 
#include "MS5611Sensor.h"
MS5611Sensor sensor(SENSOR_SDA_PIN, SENSOR_SCL_PIN, SENSOR_I2C_ADDR, SENSOR_DELAY);
#endif

SleepMonitor sleepMonitor;
GlobalConfiguration conf;
HardManager hardman;
#ifdef SENSOR
Commander com(&conf, &hardman, &sensor);
#else
Commander com(&conf, &hardman);
#endif

void setup() {
  Serial.begin(115200);
  Serial.print("Version : ");
  Serial.println(VERSION);

  sleepMonitor.begin();
  conf.begin();
  hardman.begin(conf.getDuration());  
  com.begin(conf.getMiddleCoursePosition());
  #ifdef SENSOR
  sensor.begin();
  #endif
}

void loop() {
  sleepMonitor.startLoop();

  #ifdef SENSOR
  sensor.loop();
  #endif
  hardman.loop();
  com.loop();

  #ifdef MONITOR
  unsigned long diff = sleepMonitor.getElapsedTime();
  if (diff > 2000) {
    log_printf("----\nChip flash size: %d\nFree heap: %d\nHeap fragmentation: %d\n----\n", ESP.getFlashChipRealSize(), ESP.getFreeHeap(), ESP.getHeapFragmentation());
  }
  #endif

  

  sleepMonitor.endLoop();
}
