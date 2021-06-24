#include "ConfigurationAP.h"

ConfigurationAP::ConfigurationAP() : localIp(192, 168, 1, 1), gateway(192, 168, 1, 1), subnet(255, 255, 255, 0) {
  memcpy(this->ssid, AP_SSID_PREFIX, strlen(AP_SSID_PREFIX));
  unsigned int chipId = ESP.getChipId();
  sprintf(this->ssid, "%s%08X", AP_SSID_PREFIX, chipId);
}


ConfigurationAP::~ConfigurationAP() {
  if (this->server) {
    this->server->stop();
    delete this->server;
  }

  log_printf("[ConfigurationAP::~ConfigurationAP]SoftAPDisconnect : %d", WiFi.softAPdisconnect(true));
}


void ConfigurationAP::begin() {
  log_printf(
    "[ConfigurationAP::begin]Enable AP mode... %s", 
    WiFi.enableAP(true) ? "success" : "failed"
  );
  
  log_printf(
    "[ConfigurationAP::begin]Setting soft-AP configuration... %s", 
    WiFi.softAPConfig(this->localIp, this->gateway, this->subnet) ? "success" : "failed"
  );

  log_printf(
    "[ConfigurationAP::begin]Setting soft-AP with ssid %s... %s",
    this->ssid,
    WiFi.softAP(this->ssid, AP_SSID_PASSWORD) ? "success" : "failed"
  );
  
  if (this->server) {
    delete this->server;
  }

  log_printf("[ConfigurationAP::begin]Server ip: %s", WiFi.softAPIP().toString().c_str());
  this->server = new ESP8266WebServer(localIp, 80);
  this->configurationEndpoints.begin(this->server);
  this->server->onNotFound(std::bind(&ConfigurationAP::onNotFound, this));
  this->server->begin();
  
}


void ConfigurationAP::onNotFound() {
  log_printf("[ConfigurationAP::onNotFound]");
  this->server->send(404, "text/html", "<h1>Not found :(</h1>");
}


void ConfigurationAP::loop() {
  if (this->server) {
    this->server->handleClient();    
  }
}


void ConfigurationAP::onWifiCredentialsDefined(ConfigurationEndpoints::WifiCredentialsDefinedCallbackFunction callback) {
  this->configurationEndpoints.onWifiCredentialsDefined(callback);
}
