#include "GlobalConfiguration.h"

GlobalConfiguration::GlobalConfiguration() {} 

void GlobalConfiguration::begin() {
  EEPROM.begin(EEPROM_BUFFER_SIZE);
  
  uint32_t sig = 0x0;
  sig = EEPROM.get<uint32_t>(EEPROM_INITIALIZED_ADDRESS, sig);

  if (sig == EEPROM_SIG) {
    log_printf("[GlobalConfiguration::begin]EEPROM initialized");
  } else {  
    log_printf("[GlobalConfiguration::begin]Initializing EEPROM");
    // clear memory
    this->writeBuffer(0x00, (const uint8_t*)"", 0, EEPROM.length());
    EEPROM.put<uint32_t>(EEPROM_INITIALIZED_ADDRESS, EEPROM_SIG);
    EEPROM.put<float>(MIDDLE_COURSE_POSITION_ADDRESS, DEFAULT_MIDDLE_COURSE_POSITION);
    EEPROM.put<unsigned long>(DURATION_ADDRESS, DEFAULT_DURATION);
    EEPROM.commit();
  }
  
  this->configured = EEPROM.get<bool>(CONFIGURED_ADDRESS, this->configured);
  this->wifiSsid = (const char*)&EEPROM[WIFI_SSID_ADDRESS];
  this->wifiPassword = (const char*)&EEPROM[WIFI_PASSWORD_ADDRESS];
  this->serverUrl = (const char*)&EEPROM[SERVER_URL_ADDRESS];
  this->duration = EEPROM.get<unsigned long>(DURATION_ADDRESS, this->duration);
  this->middleCoursePosition = EEPROM.get<float>(MIDDLE_COURSE_POSITION_ADDRESS, this->middleCoursePosition);

  log_printf(
    "[GlobalConfiguration::begin]configured: %d\nssid: %s\npassword: %s\nserverUrl: %s\ncurtain duration: %u\nmiddle course position: %0.05f",
    this->configured,
    this->wifiSsid,
    this->wifiPassword,
    this->serverUrl,
    this->duration,
    this->middleCoursePosition
  );
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

unsigned long GlobalConfiguration::getDuration() {
  return this->duration;
}

float GlobalConfiguration::getMiddleCoursePosition() {
  return this->middleCoursePosition;
}


void GlobalConfiguration::setDurationAndMiddleCoursePosition(unsigned long duration, float middleCoursePosition) {
  this->middleCoursePosition = middleCoursePosition;
  this->duration = duration;
  
  EEPROM.put<unsigned long>(DURATION_ADDRESS, this->duration);
  EEPROM.put<float>(MIDDLE_COURSE_POSITION_ADDRESS, middleCoursePosition);
  EEPROM.commit();
  
  log_printf(
    "[GlobalConfiguration::setDurationAndMiddleCoursePosition]Duration: %u, middleCoursePosition: %0.5f",
    this->duration,
    this->middleCoursePosition
  );
}

void GlobalConfiguration::setNetworkInformations(const char* ssid, const char* password, const char* serverUrl) {
  this->writeBuffer(SERVER_URL_ADDRESS, (const uint8_t*)serverUrl, strlen(serverUrl), SERVER_URL_LENGTH);
  this->writeBuffer(WIFI_SSID_ADDRESS, (const uint8_t*)ssid, strlen(ssid), WIFI_SSID_LENGTH);
  this->writeBuffer(WIFI_PASSWORD_ADDRESS, (const uint8_t*)password, strlen(password), WIFI_PASSWORD_LENGTH);
   
  this->configured = true;
  EEPROM.put<bool>(CONFIGURED_ADDRESS, this->configured);
  EEPROM.commit();
  
  log_printf(
    "[GlobalConfiguration::setNetworkInformations]ssid: %s, password: %s, serverUrl: %s, isConfigured: %d",
    this->wifiSsid,
    this->wifiPassword,
    this->serverUrl,
    this->configured
  );
}
