#ifndef COMMANDER_H
#define COMMANDER_H

#include <functional>
#include "parameters.h"
#include "GlobalConfiguration.h"
#include "HardManager.h"
#include "log.h"

#define START_DURATION_CONFIGURATION_DURATION 2000
#define START_GLOBAL_CONFIGURATION_DURATION 5000

class Commander {
  public:
    Commander(GlobalConfiguration* globalConf, HardManager* hardman);
    void begin(float middleCoursePosition = DEFAULT_MIDDLE_COURSE_POSITION);
    void loop();
    void onCloseButton(bool down, unsigned long since, HardManager* hm);
    void onStopButton(bool down, unsigned long since, HardManager* hm);
    void onOpenButton(bool down, unsigned long since, HardManager* hm);
    void onPositionChanged(float position, HardManager* hm);
    void onUpRelay(bool on, unsigned long since, HardManager* hm);
    void onDownRelay(bool on, unsigned long since, HardManager* hm);

  protected:
    void onStartDurationConfiguration();
    void onStartConfigurationMode(); // @todo
  
  private:
    GlobalConfiguration* conf;
    HardManager* hardman;
    bool isConfiguringDuration = false;
    float middleCoursePosition = DEFAULT_MIDDLE_COURSE_POSITION;
    unsigned long middleCourseTime = 0;
    unsigned long lastStopButtonUp = 0;
};

#endif
