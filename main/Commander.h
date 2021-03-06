#ifndef COMMANDER_H
#define COMMANDER_H

#include <functional>
#include "parameters.h"
#include "GlobalConfiguration.h"
#include "HardManager.h"
#include "NormalSTA.h"
#include "ConfigurationAP.h"
#include "AbstractSensor.h"
#include "SleepMonitor.h"
#include "log.h"

#define START_DURATION_CONFIGURATION_DURATION 2000
#define START_GLOBAL_CONFIGURATION_DURATION 5000
#define RESTART_DURATION 10000

class Commander {
  public:
    enum Mode {
      INITIALIZING = 0,
      NORMAL = 1,
      CONFIGURATION = 2  
    };
    
    Commander(GlobalConfiguration* globalConf, HardManager* hardman, SleepMonitor* sleepMonitor, AbstractSensor* sensor);
    void begin(float middleCoursePosition = DEFAULT_MIDDLE_COURSE_POSITION);
    void loop();
    void onCloseButton(bool down, unsigned long since, HardManager* hm);
    void onStopButton(bool down, unsigned long since, HardManager* hm);
    void onOpenButton(bool down, unsigned long since, HardManager* hm);
    void onPositionChanged(float position, HardManager* hm);
    void onUpRelay(bool on, unsigned long since, HardManager* hm);
    void onDownRelay(bool on, unsigned long since, HardManager* hm);
    void onConfigurationDefined(const char* ssid, const char* password, const char* name);
    void onWifiConnectionSuccess(int status);
    void onWifiConnectionFailed(int status);
    Mode getMode();
    
  protected:
    void onStartDurationConfiguration();
    void setMode(Mode mode);
  
  private:
    void changeMode();
    SleepMonitor* sleepMonitor = nullptr;
    AbstractSensor* sensor = nullptr;
    GlobalConfiguration* conf = nullptr;
    HardManager* hardman = nullptr;
    ConfigurationAP* configurationAp = nullptr;
    NormalSTA* normalSta = nullptr;
    Mode mode = Mode::NORMAL;
    Mode previousMode = Mode::INITIALIZING;
    bool isConfiguringDuration = false;
    float middleCoursePosition = DEFAULT_MIDDLE_COURSE_POSITION;
    unsigned long middleCourseTime = 0;
    unsigned long lastStopButtonUp = 0;
};

#endif
