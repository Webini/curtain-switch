#ifndef SLEEP_MONITOR_H
#define SLEEP_MONITOR_H

#ifndef WORKING_MODE_DURATION
#define WORKING_MODE_DURATION 5000ul
#endif

#ifndef SLEEP_DELAY 
#define SLEEP_DELAY 300
#endif

#ifndef DETECT_ACTIVITY_DELAY_MIN
#define DETECT_ACTIVITY_DELAY_MIN 15
#endif

#include <Arduino.h>
#include "log.h"

class SleepMonitor {
  public:
    SleepMonitor(unsigned long sleepDelay = SLEEP_DELAY, unsigned long workDuration = WORKING_MODE_DURATION, unsigned long detectActivityAfter = DETECT_ACTIVITY_DELAY_MIN);
    void begin();
    void startLoop();
    void endLoop();
    void notifyActivity();
    bool isInSleepMode();
    unsigned long getElapsedTime();
    unsigned long getElapsedTimeSinceLastActivity();

  private:
    unsigned long detectActivityAfter = 0;
    unsigned long workDuration = 0;
    unsigned long sleepDelay = 0;
    unsigned long loopStartedAt = 0;
    unsigned long lastActivityAt = 0;
};

#endif
