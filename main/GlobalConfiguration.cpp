#include "GlobalConfiguration.h"

GlobalConfiguration::GlobalConfiguration() : configured(false) {} 


void GlobalConfiguration::begin() {
  EEPROM.begin(EEPROM_BUFFER_SIZE);
  
  uint32_t sig = 0x0;
  sig = EEPROM.get<uint32_t>(EEPROM_INITIALIZED_ADDRESS, sig);

  if (sig == EEPROM_SIG) {
    Serial.println("EEPROM initialized");
  } else {
    Serial.println("Initializing EEPROM");
    // clear memory
    this->writeBuffer(0x00, (const uint8_t*)"", 0, EEPROM.length());
    EEPROM.put<uint32_t>(EEPROM_INITIALIZED_ADDRESS, EEPROM_SIG);
    EEPROM.commit();
  }

  this->configured = EEPROM.get<bool>(CONFIGURED_ADDRESS, this->configured);
  this->wifiSsid = (const char*)&EEPROM[WIFI_SSID_ADDRESS];
  this->wifiPassword = (const char*)&EEPROM[WIFI_PASSWORD_ADDRESS];
  this->serverUrl = (const char*)&EEPROM[SERVER_URL_ADDRESS];
}

void GlobalConfiguration::writeBuffer(const int address, const uint8_t* str, size_t strLen, size_t maxLen) {
  for (size_t i = 0; i < maxLen - 2; i++) {
    if (i < strLen) {
     EEPROM.put<uint8_t>(address + i, str[i]);     
    } else {
      EEPROM.put<uint8_t>(address + i, 0x00);
    }
  }
  
  EEPROM.put<uint8_t>(address + maxLen - 1, 0x00);
}

const char* GlobalConfiguration::getWifiSsid() {
  return this->wifiSsid;
}

const char* GlobalConfiguration::getWifiPassword() {
  return this->wifiPassword;
}

const char* GlobalConfiguration::getServerUrl() {
  return this->serverUrl;
}

bool GlobalConfiguration::isConfigured() {
  return this->configured;
}

void GlobalConfiguration::setWifiSsid(const char* ssid) {
  this->writeBuffer(WIFI_SSID_ADDRESS, (const uint8_t*)ssid, strlen(ssid), WIFI_SSID_LENGTH);
  EEPROM.commit(); 
}

void GlobalConfiguration::setWifiPassword(const char* password) {
  this->writeBuffer(WIFI_PASSWORD_ADDRESS, (const uint8_t*)password, strlen(password), WIFI_PASSWORD_LENGTH);
  EEPROM.commit(); 
}

void GlobalConfiguration::setServerUrl(const char* url) {
  this->writeBuffer(SERVER_URL_ADDRESS, (const uint8_t*)url, strlen(url), SERVER_URL_LENGTH);
  EEPROM.commit(); 
}

void GlobalConfiguration::setIsConfigured(bool configured) {
  EEPROM.put<bool>(CONFIGURED_ADDRESS, this->configured);
  this->configured = configured;
  EEPROM.commit(); 
}
