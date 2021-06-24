#include <EEPROM.h>
#include <ESP8266WiFi.h>
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
  WiFi.mode(WIFI_AP_STA);
  Serial.begin(115200);
  Serial.print("Version : ");
  Serial.println(VERSION);
  
  conf.begin();
  hardman.begin(conf.getDuration());  
  com.begin(conf.getMiddleCoursePosition());
}

void loop() {
  unsigned long start = millis();
  
  hardman.loop();
  com.loop();

  #ifdef MONITOR
  unsigned long diff = millis() - lastMonitor;
  if (diff > 2000) {
    log_printf("----\nChip flash size: %d\nFree heap: %d\nHeap fragmentation: %d\n----\n", ESP.getFlashChipRealSize(), ESP.getFreeHeap(), ESP.getHeapFragmentation());
    lastMonitor = millis();  
  }
  #endif
  
  unsigned long duration = millis() - start;
  if (duration < DELAY_MIN) {  
    delay(DELAY_MIN - duration);
  }
}
