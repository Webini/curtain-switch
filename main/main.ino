#include <EEPROM.h>
#include "log.h"
#include "GlobalConfiguration.h"
#include "Commander.h"
#include "HardManager.h"


GlobalConfiguration conf;
HardManager hardman;
Commander com(&conf, &hardman);

void setup() {
  Serial.begin(115200);
  
  conf.begin();
  
  hardman.begin(conf.getDuration());
  
  com.begin(conf.getMiddleCoursePosition());
}

void loop() {
  hardman.loop();
  com.loop();
  delay(50);
}
