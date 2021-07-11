#include "SleepMonitor.h"

SleepMonitor::SleepMonitor(unsigned long _sleepDelay, unsigned long _workDuration, unsigned long _detectActivityAfter) : sleepDelay(_sleepDelay), workDuration(_workDuration), detectActivityAfter(_detectActivityAfter) {}


void SleepMonitor::begin() {
  log_printf(
    "[SleepMonitor::begin]Sleep delay : %d , work duration : %d, detect activity after : %d", 
    this->sleepDelay, this->workDuration, this->detectActivityAfter
  );
  this->notifyActivity();
}


void SleepMonitor::endLoop() {
  unsigned long now = millis();
  unsigned long duration = now - this->loopStartedAt;

  if (duration > this->detectActivityAfter) {
    this->notifyActivity();
  }

  if (this->lastActivityAt && millis() - this->lastActivityAt > this->workDuration) {
    log_printf("[SleepMonitor::endLoop]Inactivity detected (%d, %d, %d), leaving working mode", this->lastActivityAt, now - this->lastActivityAt, now);
    this->lastActivityAt = 0;
  }

  if (!this->lastActivityAt && duration < this->sleepDelay) {
    // log_printf("[SleepMonitor::endLoop]Sleep : %d (loop duration %d)", this->sleepDelay - duration, duration);
    delay(this->sleepDelay - duration);
  }
}


void SleepMonitor::startLoop() {
  this->loopStartedAt = millis();
}


void SleepMonitor::notifyActivity() {
  if (this->lastActivityAt == 0) {
    log_printf("[SleepMonitor::notifyActivity]Activity detected, entering working mode (work cycle %d)", millis() - this->loopStartedAt);
  }
  
  this->lastActivityAt = millis();
}


bool SleepMonitor::isInSleepMode() {
  return this->lastActivityAt == 0;
}


unsigned long SleepMonitor::getElapsedTime() {
  return millis() - this->loopStartedAt;
}

unsigned long SleepMonitor::getElapsedTimeSinceLastActivity() {
  return millis() - this->lastActivityAt;
}
