#include "ConfigurationAP.h"

ConfigurationAP::ConfigurationAP() : localIp(192, 168, 1, 1), gateway(192, 168, 1, 1), subnet(255, 255, 255, 0), server(localIp) {
  memcpy(this->ssid, AP_SSID_PREFIX, strlen(AP_SSID_PREFIX));
  unsigned int chipId = ESP.getChipId();
  log_printf("Received info %08X", chipId);
  sprintf(this->ssid, "%s%08X", AP_SSID_PREFIX, chipId);
}


ConfigurationAP::~ConfigurationAP() {
  WiFi.softAPdisconnect();
}


void ConfigurationAP::begin() {
  log_printf(
    "[ConfigurationAP::begin]Setting soft-AP configuration... %s", 
    WiFi.softAPConfig(this->localIp, this->gateway, this->subnet) ? "success" : "failed"
  );

  log_printf(
    "[ConfigurationAP::begin]Setting soft-AP with ssid %s... %s",
    this->ssid,
    WiFi.softAP(this->ssid, AP_SSID_PASSWORD) ? "success" : "failed"
  );
  
  log_printf("[ConfigurationAP::begin]Server ip: %s", WiFi.softAPIP().toString().c_str());
  this->server.begin();
}


void ConfigurationAP::loop() {
  this->server.loop();
}


void ConfigurationAP::onWifiCredentialsDefined(ConfigurationWebServer::WifiCredentialsDefinedCallbackFunction callback) {
  this->server.onWifiCredentialsDefined(callback);
}
