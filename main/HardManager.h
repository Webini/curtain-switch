#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include <Arduino.h>
#include <functional>
#include "parameters.h"
#include "log.h"

#define UNKNOWN_CURRENT_POSITION -1

#define DIRECTION_OPEN 1
#define DIRECTION_CLOSE -1

#define POSITION_OPENED 1.0
#define POSITION_CLOSED 0.0

#define ABS(v) (v < 0 ? -(v) : v)

class HardManager {
  public:
    enum ReturnCode {
      NOTHING_TODO = 2,
      SUCCESS = 1,
      INVALID_POSITION = 0,
      INITIALIZING = -1
    };
    
    enum LedMode {
      BLINK_SLOW = 8,
      BLINK_FAST = 4,
      BLINK = 2,
      ON = 1,
      OFF = 0
    };
    
    typedef std::function<void(bool, unsigned long, HardManager*)> BtnStateChangeCallbackFunction;
    typedef std::function<void(bool, unsigned long, HardManager*)> RelayStateChangeCallbackFunction;
    typedef std::function<void(float, HardManager*)> PositionChangedCallbackFunction;
    
    HardManager(
      byte ledPin = DEFAULT_LED_PIN, byte upRelayPin = DEFAULT_UP_RELAY_PIN, 
      byte openBtnPin = DEFAULT_OPEN_BTN_PIN, byte stopBtnPin = DEFAULT_STOP_BTN_PIN, 
      byte downRelayPin = DEFAULT_DOWN_RELAY_PIN, byte closeBtnPin = DEFAULT_CLOSE_BTN_PIN
    );

    /**
     * Initialize hardware
     * @param duration duration in ms
     */
    void begin(unsigned long duration = DEFAULT_DURATION);

    /**
     * Get current position value
     */
    float getCurrentPositionValue();
    /**
     * Set current position value, carefully use this api, you must call stop() before changing this value
     */
    void setCurrentPositionValue(float position);
    /**
     * Set opening / closing duration in millisec, carefully use this api, you must call stop() before changing this value
     */
    void setDuration(unsigned long duration);
    /**
     * Get opening / closing duration in millisec
     */
    unsigned long getDuration();
    /**
     * @param position curtain position 0 (closed) to 1 (opened)
     */
    ReturnCode setPosition(float position); 
    /**
     * Stop opening / closing
     */
    ReturnCode stop();
    /**
     * Set led mode
     */
    void setLedMode(LedMode mode);
    /**
     * Hardware monitoring loop
     */
    void loop();
    /**
     * Get led mode
     */
    LedMode getLedMode();
    bool isUpRelayOn();
    bool isDownRelayOn();
    bool isCloseBtnDown();
    bool isOpenBtnDown();
    bool isStopBtnDown();
    bool isInitializing();
    void onOpenBtnStateChange(BtnStateChangeCallbackFunction callback);
    void onCloseBtnStateChange(BtnStateChangeCallbackFunction callback);
    void onStopBtnStateChange(BtnStateChangeCallbackFunction callback);
    void onUpRelayStateChange(RelayStateChangeCallbackFunction callback);
    void onDownRelayStateChange(RelayStateChangeCallbackFunction callback);
    void onPositionChanged(PositionChangedCallbackFunction callback);

  protected:
    bool setUpRelayState(bool on);
    bool setDownRelayState(bool on);
    
  private:
    bool initializing = false;
    void updateCurrentPosition();
    void blinkLed();
    byte ledPin;
    byte upRelayPin;
    byte openBtnPin;
    byte stopBtnPin;
    byte downRelayPin;
    byte closeBtnPin;
    unsigned long closeBtnDownSince = 0;
    unsigned long stopBtnDownSince = 0;
    unsigned long openBtnDownSince = 0;
    unsigned long upRelayOnSince = 0;
    unsigned long downRelayOnSince = 0;
    unsigned long stopDownRelayAfter = 0;
    unsigned long stopUpRelayAfter = 0;
    unsigned long duration = DEFAULT_DURATION; // opening or closing duration in milliseconds
    unsigned long setPositionSince = 0;
    float currentPosition = POSITION_CLOSED; // current curtain position 
    float initialPosition = POSITION_CLOSED; // initial curtain position (before up or down relay activation)
    LedMode ledMode = LedMode::OFF;
    unsigned long ledLastStateChangeAt = 0;
    bool ledOn = false;
    BtnStateChangeCallbackFunction closeBtnStateChangeCallback = nullptr;
    BtnStateChangeCallbackFunction stopBtnStateChangeCallback = nullptr;
    BtnStateChangeCallbackFunction openBtnStateChangeCallback = nullptr;
    RelayStateChangeCallbackFunction upRelayStateChangeCallback = nullptr;
    RelayStateChangeCallbackFunction downRelayStateChangeCallback = nullptr;
    PositionChangedCallbackFunction positionChangedCallback = nullptr;
};

#endif
