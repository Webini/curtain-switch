#ifndef GLOBAL_CONFIGURATION_H
#define GLOBAL_CONFIGURATION_H

#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_BUFFER_SIZE 512
#define EEPROM_INITIALIZED_ADDRESS 0x0
#define EEPROM_SIG 0x0a0b0c0d
#define EEPROM_SIG_LENGTH 4
#define CONFIGURED_ADDRESS EEPROM_INITIALIZED_ADDRESS + EEPROM_SIG_LENGTH
#define CONFIGURED_LENGTH 1
#define WIFI_SSID_ADDRESS CONFIGURED_ADDRESS + CONFIGURED_LENGTH
#define WIFI_SSID_LENGTH 34
#define WIFI_PASSWORD_ADDRESS WIFI_SSID_ADDRESS + WIFI_SSID_LENGTH
#define WIFI_PASSWORD_LENGTH 64
#define SERVER_URL_ADDRESS WIFI_PASSWORD_ADDRESS + WIFI_PASSWORD_LENGTH 
#define SERVER_URL_LENGTH 150

class GlobalConfiguration {
  public:
    GlobalConfiguration();
    void begin();
    const char* getWifiSsid();
    const char* getWifiPassword();
    const char* getServerUrl();
    bool isConfigured();
    void setWifiPassword(const char* password);
    void setWifiSsid(const char* ssid);
    void setServerUrl(const char* serverUrl);
    void setIsConfigured(bool configured);

  private:
    bool configured;
    const char* wifiSsid;
    const char* wifiPassword;
    const char* serverUrl;
    void writeBuffer(const int address, const uint8_t* str, size_t strLen, size_t maxLen);
};

#endif
