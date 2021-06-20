#include <EEPROM.h>
#include "parameters.h"
#include "log.h"
#include "GlobalConfiguration.h"
#include "Commander.h"
#include "HardManager.h"

GlobalConfiguration conf;
HardManager hardman;
Commander com(&conf, &hardman);

#ifdef MONITOR
unsigned long lastMonitor;
#endif

void setup() {
  Serial.begin(115200);
  
  conf.begin();
  
  hardman.begin(conf.getDuration());
  
  com.begin(conf.getMiddleCoursePosition());
}

void loop() {
  hardman.loop();
  com.loop();

  #ifdef MONITOR
  unsigned long diff = millis() - lastMonitor;
  if (diff > 2000) {
    log_printf("----\nChip flash size: %d\nFree heap: %d\nHeap fragmentation: %d\n----\n", ESP.getFlashChipRealSize(), ESP.getFreeHeap(), ESP.getHeapFragmentation());
    lastMonitor = millis();  
  }
  #endif
  delay(50);
}
