#include "HardManager.h"

HardManager::HardManager(
  byte _ledPin, byte _upRelayPin, 
  byte _openBtnPin, byte _stopBtnPin, 
  byte _downRelayPin, byte _closeBtnPin
) : ledPin(_ledPin), upRelayPin(_upRelayPin), openBtnPin(_openBtnPin), stopBtnPin(_stopBtnPin), 
    downRelayPin(_downRelayPin), closeBtnPin(_closeBtnPin) {
}


void HardManager::begin(unsigned long duration) {
  this->duration = duration;
  
  pinMode(this->ledPin, OUTPUT);
  digitalWrite(this->ledPin, LOW); 
  
  pinMode(this->upRelayPin, OUTPUT);
  digitalWrite(this->upRelayPin, LOW);
  
  pinMode(this->downRelayPin, OUTPUT);
  digitalWrite(this->downRelayPin, LOW);
  
  pinMode(this->closeBtnPin, INPUT);
  pinMode(this->openBtnPin, INPUT);
  pinMode(this->stopBtnPin, INPUT);

  this->setPosition(POSITION_OPENED);
  this->initializing = true;
}


void HardManager::loop() {
  bool closeBtnDown = digitalRead(this->closeBtnPin) == LOW;
  bool stopBtnDown = digitalRead(this->stopBtnPin) == LOW;
  bool openBtnDown = digitalRead(this->openBtnPin) == LOW;

  if (closeBtnDown != !!this->closeBtnDownSince) {
    unsigned long oldSince = this->closeBtnDownSince;
    this->closeBtnDownSince = (closeBtnDown ? millis() : 0);
    if (this->closeBtnStateChangeCallback) { 
      this->closeBtnStateChangeCallback(closeBtnDown, closeBtnDown ? this->closeBtnDownSince : oldSince, this);
    }
  }
  
  if (stopBtnDown != !!this->stopBtnDownSince) {
    unsigned long oldSince = this->stopBtnDownSince;
    this->stopBtnDownSince = (stopBtnDown ? millis() : 0);
    if (this->stopBtnStateChangeCallback) { 
      this->stopBtnStateChangeCallback(stopBtnDown, stopBtnDown ? this->stopBtnDownSince : oldSince, this);
    }
  }

  if (openBtnDown != !!this->openBtnDownSince) {
    unsigned long oldSince = this->openBtnDownSince;
    this->openBtnDownSince = (openBtnDown ? millis() : 0);
    if (this->openBtnStateChangeCallback) { 
      this->openBtnStateChangeCallback(openBtnDown, openBtnDown ? this->openBtnDownSince : oldSince, this);
    }
  }

  this->updateCurrentPosition();

  // if up relay time is expired
  if (this->stopUpRelayAfter && this->stopUpRelayAfter < millis() || this->upRelayOnSince && (millis() - this->upRelayOnSince) > this->duration) {
    this->setUpRelayState(false);
  }

  // if down relay time is expired
  if (this->stopDownRelayAfter && this->stopDownRelayAfter < millis() || this->downRelayOnSince && (millis() - this->downRelayOnSince) > this->duration) {  
    this->setDownRelayState(false);
  }

  this->blinkLed();
}


void HardManager::updateCurrentPosition() {
  unsigned long now = millis();
  unsigned long elapsedTime = 0;
  float direction = DIRECTION_OPEN;
  
  if (this->stopDownRelayAfter) {
    elapsedTime = now - this->setPositionSince;
    direction = DIRECTION_CLOSE;
  } else if (this->stopUpRelayAfter) {
    elapsedTime = now - this->setPositionSince;
  } else {
    return;
  }

  float newCurrentPosition = this->initialPosition + direction * ((float)elapsedTime / (float)this->duration);

  if (newCurrentPosition > 1) {
    newCurrentPosition = 1;
  } else if (newCurrentPosition < 0) {
    newCurrentPosition = 0;
  } 
  
  if (this->currentPosition == newCurrentPosition) {
    return;
  }

  log_printf("[HardManager::updateCurrentPosition] new position: %0.5f, currentPosition: %0.5f, elapsedTime: %u, direction : %f", newCurrentPosition, this->currentPosition, elapsedTime, direction);
  
  this->currentPosition = newCurrentPosition;
  
  // initializing opening
  if (this->initializing && this->currentPosition == 1) {
    this->initializing = false;
  }
  
  if (this->positionChangedCallback) {
    this->positionChangedCallback(this->currentPosition, this);
  }
}


HardManager::ReturnCode HardManager::setPosition(float position) {
  if (this->initializing) {
    return ReturnCode::INITIALIZING;
  }
  
  if (position < 0 || position > 1) {
    return ReturnCode::INVALID_POSITION;
  }

  this->updateCurrentPosition();
  
  float direction = this->currentPosition > position ? DIRECTION_CLOSE : DIRECTION_OPEN;
  float diff = ABS(this->currentPosition - position);
  unsigned long time = (unsigned long)(diff * this->duration);

  if (time == 0) {
    return ReturnCode::NOTHING_TODO;
  }

  log_printf("[HardManager::setPosition] direction: %f, currentPosition: %0.5f, diff: %0.5f, time %u, initialPosition: %0.5f \n", direction, this->currentPosition, diff, time, this->initialPosition);

  this->initialPosition = this->currentPosition;

  if (direction == DIRECTION_OPEN) {
    this->setDownRelayState(false);
    this->setPositionSince = millis();
    this->stopUpRelayAfter = this->setPositionSince + time;
    this->setUpRelayState(true);
  } 
  
  if (direction == DIRECTION_CLOSE) {
    this->setUpRelayState(false);
    this->setPositionSince = millis();
    this->stopDownRelayAfter = this->setPositionSince + time;
    this->setDownRelayState(true);
  }

  return ReturnCode::SUCCESS;
}


bool HardManager::setUpRelayState(bool on) {
  if (on == !!this->upRelayOnSince) {
    return false;
  }
  
  if (!on) {
    this->updateCurrentPosition();
    this->stopUpRelayAfter = 0;
  }
  
  unsigned long oldSince = this->upRelayOnSince;
  this->upRelayOnSince = on ? millis() : 0;
  digitalWrite(this->upRelayPin, on ? HIGH : LOW);
  
  if (this->upRelayStateChangeCallback) {
    this->upRelayStateChangeCallback(on, on ? this->upRelayOnSince : oldSince, this);
  }
  
  return true;
}


bool HardManager::setDownRelayState(bool on) {
  if (on == !!this->downRelayOnSince) {
    return false;
  }
  
  if (!on) {
    this->updateCurrentPosition();
    this->stopDownRelayAfter = 0;
  }
  
  unsigned long oldSince = this->downRelayOnSince;
  this->downRelayOnSince = on ? millis() : 0;
  digitalWrite(this->downRelayPin, on ? HIGH : LOW);
  
  if (this->downRelayStateChangeCallback) {
    this->downRelayStateChangeCallback(on, on ? this->downRelayOnSince : oldSince, this);
  }

  return true;
}


HardManager::ReturnCode HardManager::stop() {
  if (this->initializing) {
    return ReturnCode::INITIALIZING;
  }
  
  this->setUpRelayState(false);
  this->setDownRelayState(false);

  return ReturnCode::SUCCESS;
}


float HardManager::getCurrentPositionValue() {
  return this->currentPosition;
}


void HardManager::setCurrentPositionValue(float position) {
  this->currentPosition = this->initialPosition = position;
}


void HardManager::setDuration(unsigned long duration) {
  this->duration = duration;
}


unsigned long HardManager::getDuration() {
  return this->duration;
}


void HardManager::setLedMode(LedMode mode) {
  this->ledMode = mode;
  
  if (this->ledMode == LedMode::ON) {
    digitalWrite(this->ledPin, HIGH);
    this->ledOn = true;
  } else if (this->ledMode == LedMode::OFF) {
    digitalWrite(this->ledPin, LOW);
    this->ledOn = false;
  } else {    
    digitalWrite(this->ledPin, HIGH);
    this->ledLastStateChangeAt = millis();
    this->ledOn = true;
  }
}


void HardManager::blinkLed() {
  if ((this->ledMode & (LedMode::BLINK | LedMode::BLINK_FAST | LedMode::BLINK_SLOW)) == 0) {
    return;
  }
  
  unsigned long waitingDelay = BLINK_INTERVAL;
  
  if (this->ledMode == LedMode::BLINK_FAST) {
    waitingDelay = BLINK_FAST_INTERVAL;
  } else if (this->ledMode == LedMode::BLINK_SLOW) {
    waitingDelay = BLINK_SLOW_INTERVAL;  
  }
  
  unsigned long now = millis();
  if (this->ledLastStateChangeAt + waitingDelay < now) {
    this->ledOn = !this->ledOn;
    digitalWrite(this->ledPin, this->ledOn ? HIGH : LOW);
    this->ledLastStateChangeAt = now;
  }
}


HardManager::LedMode HardManager::getLedMode() {
  return this->ledMode;
}


bool HardManager::isUpRelayOn() {
  return !!this->upRelayOnSince; 
}


bool HardManager::isDownRelayOn() {
  return !!this->downRelayOnSince;
}


bool HardManager::isCloseBtnDown() {
  return !!this->closeBtnDownSince;
}


bool HardManager::isOpenBtnDown() {
  return !!this->openBtnPin;
}


bool HardManager::isStopBtnDown() {
  return !!this->stopBtnDownSince;
}


bool HardManager::isInitializing() {
  return this->initializing;
}

void HardManager::onPositionChanged(PositionChangedCallbackFunction callback) {
  this->positionChangedCallback = callback;
}

void HardManager::onOpenBtnStateChange(BtnStateChangeCallbackFunction callback) {
  this->openBtnStateChangeCallback = callback;
}

void HardManager::onCloseBtnStateChange(BtnStateChangeCallbackFunction callback) {
  this->closeBtnStateChangeCallback = callback;
}

void HardManager::onStopBtnStateChange(BtnStateChangeCallbackFunction callback) {
  this->stopBtnStateChangeCallback = callback;
}

void HardManager::onUpRelayStateChange(RelayStateChangeCallbackFunction callback) {
  this->upRelayStateChangeCallback = callback;
}

void HardManager::onDownRelayStateChange(RelayStateChangeCallbackFunction callback) {
  this->downRelayStateChangeCallback = callback;
}
