#include "HardManager.h"

HardManager::HardManager(
  unsigned int _duration,
  byte _ledPin, byte _upRelayPin, 
  byte _openBtnPin, byte _stopBtnPin, 
  byte _downRelayPin, byte _closeBtnPin
) : ledPin(_ledPin), upRelayPin(_upRelayPin), openBtnPin(_openBtnPin), stopBtnPin(_stopBtnPin), 
    downRelayPin(_downRelayPin), closeBtnPin(_closeBtnPin), duration(_duration) {
}


void HardManager::begin() {
  pinMode(this->ledPin, OUTPUT);
  digitalWrite(this->ledPin, LOW); 
  
  pinMode(this->upRelayPin, OUTPUT);
  digitalWrite(this->upRelayPin, LOW);
  
  pinMode(this->downRelayPin, OUTPUT);
  digitalWrite(this->downRelayPin, LOW);
  
  pinMode(this->closeBtnPin, INPUT);
  pinMode(this->openBtnPin, INPUT);
  pinMode(this->stopBtnPin, INPUT);
}


void HardManager::loop() {
  unsigned long now = millis();
  bool closeBtnDown = digitalRead(this->closeBtnPin) == LOW;
  bool stopBtnDown = digitalRead(this->stopBtnPin) == LOW;
  bool openBtnDown = digitalRead(this->openBtnPin) == LOW;

  if (closeBtnDown != !!this->closeBtnDownSince) {
    unsigned long oldSince = this->closeBtnDownSince;
    this->closeBtnDownSince = (closeBtnDown ? now : 0);
    if (this->closeBtnStateChangeCallback) { 
      this->closeBtnStateChangeCallback(closeBtnDown, closeBtnDown ? this->closeBtnDownSince : oldSince, this);
    }
  }
  
  if (stopBtnDown != !!this->stopBtnDownSince) {
    unsigned long oldSince = this->stopBtnDownSince;
    this->stopBtnDownSince = (stopBtnDown ? now : 0);
    if (this->stopBtnStateChangeCallback) { 
      this->stopBtnStateChangeCallback(stopBtnDown, stopBtnDown ? this->stopBtnDownSince : oldSince, this);
    }
  }

  if (openBtnDown != !!this->openBtnDownSince) {
    unsigned long oldSince = this->openBtnDownSince;
    this->openBtnDownSince = (openBtnDown ? now : 0);
    if (this->openBtnStateChangeCallback) { 
      this->openBtnStateChangeCallback(openBtnDown, openBtnDown ? this->openBtnDownSince : oldSince, this);
    }
  }

  // if up relay time is expired
  if (this->stopUpRelayAfter && this->stopUpRelayAfter < now || now - this->upRelayOnSince > this->duration) {
    this->setUpRelayState(false);
  }

  // if down relay time is expired
  if (this->stopDownRelayAfter && this->stopDownRelayAfter < now || now - this->downRelayOnSince > this->duration) {
    this->setDownRelayState(false);
  }

  this->blinkLed();
}


void HardManager::open(byte percent) {
  this->setDownRelayState(false);
  this->stopUpRelayAfter = millis() + (unsigned long)(this->duration * (float)(percent / 100));
  this->setUpRelayState(true);
}


void HardManager::close(byte percent) {
  this->setUpRelayState(false);
  this->stopDownRelayAfter = millis() + (unsigned long)(this->duration * ((float)percent / 100));
  this->setDownRelayState(true);
}


void HardManager::setUpRelayState(bool on) {
  if (on != !!this->upRelayOnSince) {
    unsigned long oldSince = this->upRelayOnSince;
    this->upRelayOnSince = on ? millis() : 0;
    digitalWrite(this->upRelayPin, on ? HIGH : LOW);
    if (this->upRelayStateChangeCallback) {
      this->upRelayStateChangeCallback(on, on ? this->upRelayOnSince : oldSince, this);
    }
    if (!on) {
      this->stopUpRelayAfter = 0;
    }
  }
}


void HardManager::setDownRelayState(bool on) {
  if (on != !!this->downRelayOnSince) {
    unsigned long oldSince = this->downRelayOnSince;
    this->downRelayOnSince = on ? millis() : 0;
    digitalWrite(this->downRelayPin, on ? HIGH : LOW);
    if (this->downRelayStateChangeCallback) {
      this->downRelayStateChangeCallback(on, on ? this->downRelayOnSince : oldSince, this);
    }
    if (!on) {
      this->stopDownRelayAfter = 0;
    }
  }
}


void HardManager::stop() {
  this->setUpRelayState(false);
  this->setDownRelayState(false);
}


/**
 * Set opening / closing duration in millisec
 */
void HardManager::setDuration(unsigned int duration) {
  this->duration = duration;
}

/**
 * Get opening / closing duration in millisec
 */
unsigned int HardManager::getDuration() {
  return this->duration;
}

void HardManager::setLedMode(LedMode mode) {
  this->ledMode = mode;
  
    Serial.print("SET LED MODE");
    Serial.println(mode);
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
  if (this->ledMode != LedMode::BLINK) {
    return;
  }

  unsigned long now = millis();
  if (this->ledLastStateChangeAt + BLINK_INTERVAL < now) {
    Serial.println("Changing");
    this->ledOn = !this->ledOn;
    digitalWrite(this->ledPin, this->ledOn ? HIGH : LOW);
    this->ledLastStateChangeAt = now;
  }
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
