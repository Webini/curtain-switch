#ifndef GLOBAL_CONFIGURATION_H
#define GLOBAL_CONFIGURATION_H

#include <Arduino.h>
#include <EEPROM.h>
#include "parameters.h"
#include "log.h"

#define EEPROM_INITIALIZED_ADDRESS 0x0
#define EEPROM_SIG 0x0a0b0c0d
#define EEPROM_SIG_LENGTH 4
#define CONFIGURED_ADDRESS EEPROM_INITIALIZED_ADDRESS + EEPROM_SIG_LENGTH
#define CONFIGURED_LENGTH 1
#define DURATION_ADDRESS CONFIGURED_ADDRESS + CONFIGURED_LENGTH
#define DURATION_LENGTH sizeof(unsigned long)
#define WIFI_SSID_ADDRESS DURATION_ADDRESS + DURATION_LENGTH
#define WIFI_SSID_LENGTH 34
#define WIFI_PASSWORD_ADDRESS WIFI_SSID_ADDRESS + WIFI_SSID_LENGTH
#define WIFI_PASSWORD_LENGTH 64
#define SERVER_URL_ADDRESS WIFI_PASSWORD_ADDRESS + WIFI_PASSWORD_LENGTH 
#define SERVER_URL_LENGTH 150
#define MIDDLE_COURSE_POSITION_ADDRESS SERVER_URL_ADDRESS + SERVER_URL_LENGTH
#define MIDDLE_COURSE_POSITION_LENGTH sizeof(float)

#define EEPROM_BUFFER_SIZE MIDDLE_COURSE_POSITION_ADDRESS + MIDDLE_COURSE_POSITION_LENGTH + 1

class GlobalConfiguration {
  public:
    GlobalConfiguration();
    void begin();
    const char* getWifiSsid();
    const char* getWifiPassword();
    const char* getServerUrl();
    bool isConfigured();
    unsigned long getDuration();
    float getMiddleCoursePosition();
    void setDurationAndMiddleCoursePosition(unsigned long duration, float middleCoursePosition);
    void setNetworkInformations(const char* ssid, const char* password, const char* serverUrl); 

  private:
    bool configured = false;
    unsigned long duration = DEFAULT_DURATION;
    float middleCoursePosition = DEFAULT_MIDDLE_COURSE_POSITION;
    const char* wifiSsid = "";
    const char* wifiPassword = "";
    const char* serverUrl = "";
    void writeBuffer(const int address, const uint8_t* str, size_t strLen, size_t maxLen);
};

#endif
