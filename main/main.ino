#include <EEPROM.h>
#include "parameters.h"
#include "log.h"
#include "GlobalConfiguration.h"
#include "Commander.h"
#include "HardManager.h"
#include "SleepMonitor.h"

SleepMonitor sleepMonitor;
GlobalConfiguration conf;
HardManager hardman;
Commander com(&conf, &hardman);

void setup() {
  Serial.begin(115200);
  Serial.print("Version : ");
  Serial.println(VERSION);

  sleepMonitor.begin();
  conf.begin();
  hardman.begin(conf.getDuration());  
  com.begin(conf.getMiddleCoursePosition());
}


void loop() {
  sleepMonitor.startLoop();
  
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
