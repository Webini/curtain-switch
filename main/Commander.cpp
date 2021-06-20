#include "Commander.h"

Commander::Commander(GlobalConfiguration* globalConf, HardManager* _hardman): conf(globalConf), hardman(_hardman) {
  this->hardman->onCloseBtnStateChange(std::bind(&Commander::onCloseButton, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
  this->hardman->onOpenBtnStateChange(std::bind(&Commander::onOpenButton, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
  this->hardman->onStopBtnStateChange(std::bind(&Commander::onStopButton, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

  this->hardman->onPositionChanged(std::bind(&Commander::onPositionChanged, this, std::placeholders::_1, std::placeholders::_2));
  
  this->hardman->onUpRelayStateChange(std::bind(&Commander::onUpRelay, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
  this->hardman->onDownRelayStateChange(std::bind(&Commander::onDownRelay, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void Commander::begin(float middleCoursePosition) {
  this->middleCoursePosition = middleCoursePosition;
}

void Commander::loop() {

}

void Commander::onCloseButton(bool down, unsigned long since, HardManager* hm) {
  log_printf("[Commander::onCloseButton]Down: %d, since: %u", down, since); 
  if (this->isConfiguringDuration || this->hardman->isInitializing()) {
    return;
  }
  
  this->hardman->setPosition(POSITION_CLOSED);
}


void Commander::onStopButton(bool down, unsigned long since, HardManager* hm) {
  log_printf("[Commander::onStopButton]Down: %d, since: %u", down, since); 
  if (this->hardman->isInitializing()) {
    return;
  }
  
  if (this->isConfiguringDuration) {      
    // can't stop if we are in duration conf mode, we only accept when we stop down relay (used to define curtain duration)/
    // we also leave if button is down (to avoid setting middleCourseTime to now then when released finished the duration calibration)
    if (!this->hardman->isDownRelayOn() || down) {
      return;
    }

    // save middle course time
    if (this->middleCourseTime == 0) {
      this->middleCourseTime = millis();
      log_printf("[Commander::onStopButton]Set middle course time: %u", this->middleCourseTime); 
      return;
    }
    
    this->hardman->stop();
    return;
  }


  if (!down) {
    unsigned long now = millis();
    unsigned long elapsed = now - since;

    if (elapsed > START_GLOBAL_CONFIGURATION_DURATION) {
      // @todo global mode configuration callback
      return;
    }
    
    if (now - this->lastStopButtonUp < DBL_CLICK_DURATION) {
      log_printf("[Commander::onStopButton]Dbl click detected"); 
      this->hardman->setPosition(this->middleCoursePosition);
      return;
    }
    
    this->lastStopButtonUp = now;
  }
  
  this->hardman->stop();
}


void Commander::onOpenButton(bool down, unsigned long since, HardManager* hm) {
  log_printf("[Commander::onOpenButton]Down: %d, since: %u", down, since); 
  if (this->isConfiguringDuration || this->hardman->isInitializing()) {
    return;
  }

  unsigned long pressDuration = millis() - since;
  if (!down && pressDuration > START_DURATION_CONFIGURATION_DURATION) {
    this->onStartDurationConfiguration();
    return;
  }

  this->hardman->setPosition(POSITION_OPENED);
}


void Commander::onStartDurationConfiguration() {
  this->middleCourseTime = 0;
  this->hardman->stop();
  this->hardman->setDuration(DEFAULT_DURATION);
  this->hardman->setCurrentPositionValue(POSITION_CLOSED);
  HardManager::ReturnCode code = this->hardman->setPosition(POSITION_OPENED);
  this->isConfiguringDuration = true;
  log_printf("[Commander::onUpRelay][configuringDuration]Start duration configuration (return code %d)", code); 
  this->hardman->setLedMode(HardManager::LedMode::BLINK);
}


void Commander::onPositionChanged(float position, HardManager* hm) {
  log_printf("[Commander::onPositionChanged]position: %0.05f", position); 
}


void Commander::onUpRelay(bool on, unsigned long since, HardManager* hm) {
  log_printf("[Commander::onUpRelay]On: %d, since: %u", on, since); 
  // relay up has finished his course, now we start measuring down relay time until user press stop btn
  if (!on && this->isConfiguringDuration) {
    log_printf("[Commander::onUpRelay][configuringDuration]relay is up, going down"); 
    this->hardman->setPosition(POSITION_CLOSED);
    this->hardman->setLedMode(HardManager::LedMode::BLINK_FAST);
  }
}


void Commander::onDownRelay(bool on, unsigned long since, HardManager* hm) {
  log_printf("[Commander::onDownRelay]On: %d, since: %u", on, since); 
  // user has pressed stop btn or relay is timed out, we can saved duration in eeprom & hard manager
  if (!on && this->isConfiguringDuration) {
    unsigned long now = millis();
    unsigned long totalDuration = now - since;
    unsigned long middleDuration = (this->middleCourseTime > 0 ? now - this->middleCourseTime : (totalDuration / 2));
    this->middleCoursePosition = (float)middleDuration / (float)totalDuration;
    log_printf("[Commander::onDownRelay][configuringDuration]new total duration: %u, middle course: %u (%0.5f)", totalDuration, middleDuration, this->middleCoursePosition); 
    this->isConfiguringDuration = false;
    this->conf->setDurationAndMiddleCoursePosition(totalDuration, this->middleCoursePosition);
    this->hardman->setDuration(totalDuration);
    this->hardman->setCurrentPositionValue(POSITION_CLOSED);
    this->hardman->setLedMode(HardManager::LedMode::OFF);
  }
}
