#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include <Arduino.h>
#include <functional>

#define DEFAULT_LED_PIN 0         // D3
#define DEFAULT_UP_RELAY_PIN 15   // D8
#define DEFAULT_OPEN_BTN_PIN 5    // D1
#define DEFAULT_STOP_BTN_PIN 3    // RX 

#define DEFAULT_DOWN_RELAY_PIN 13 // D7
#define DEFAULT_CLOSE_BTN_PIN 12  // D6

#define DEFAULT_DURATION 6000     // in milliseconds

#define BLINK_INTERVAL 600        // in milliseconds

class HardManager {
  public:
    enum LedMode {
      BLINK,
      ON,
      OFF
    };
    
    typedef std::function<void(bool, unsigned long, HardManager*)> BtnStateChangeCallbackFunction;
    typedef std::function<void(bool, unsigned long, HardManager*)> RelayStateChangeCallbackFunction;
    
    HardManager(
      unsigned int duration = DEFAULT_DURATION,
      byte ledPin = DEFAULT_LED_PIN, byte upRelayPin = DEFAULT_UP_RELAY_PIN, 
      byte openBtnPin = DEFAULT_OPEN_BTN_PIN, byte stopBtnPin = DEFAULT_STOP_BTN_PIN, 
      byte downRelayPin = DEFAULT_DOWN_RELAY_PIN, byte closeBtnPin = DEFAULT_CLOSE_BTN_PIN
    );
    
    void begin();
    void setDuration(unsigned int duration);
    unsigned int getDuration();
    void open(byte percent = 100);
    void close(byte percent = 100);
    void stop();
    void setLedMode(LedMode mode);
    void loop();
    LedMode getLedMode(); // @todo 
    bool isUpRelayOn(); // @todo 
    bool isDownRelayOn(); // @todo 
    bool isCloseBtnDown(); // @todo 
    bool isUpBtnDown(); // @todo 
    bool isStopBtndown(); // @todo 
    void onOpenBtnStateChange(BtnStateChangeCallbackFunction callback);
    void onCloseBtnStateChange(BtnStateChangeCallbackFunction callback);
    void onStopBtnStateChange(BtnStateChangeCallbackFunction callback);
    void onUpRelayStateChange(RelayStateChangeCallbackFunction callback);
    void onDownRelayStateChange(RelayStateChangeCallbackFunction callback);

  protected:
    void setUpRelayState(bool on);
    void setDownRelayState(bool on);
    
  private:
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
    unsigned int duration; // opening or closing duration in milliseconds
    LedMode ledMode = LedMode::OFF;
    unsigned long ledLastStateChangeAt = 0;
    bool ledOn = false;
    BtnStateChangeCallbackFunction closeBtnStateChangeCallback = nullptr;
    BtnStateChangeCallbackFunction stopBtnStateChangeCallback = nullptr;
    BtnStateChangeCallbackFunction openBtnStateChangeCallback = nullptr;
    RelayStateChangeCallbackFunction upRelayStateChangeCallback = nullptr;
    RelayStateChangeCallbackFunction downRelayStateChangeCallback = nullptr;
};

#endif
