#ifndef CONFIGURATION_AP_H
#define CONFIGURATION_AP_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <functional>
#include <ESP8266WebServer.h>
#include "ConfigurationEndpoints.h"
#include "parameters.h"
#include "log.h"

class ConfigurationAP {
  public:
    ConfigurationAP();
    ~ConfigurationAP();
    void begin();
    void loop();
    void onNotFound();
    void onWifiCredentialsDefined(ConfigurationEndpoints::WifiCredentialsDefinedCallbackFunction callback);
    
  private:
    char ssid[34] = {0};
    ConfigurationEndpoints configurationEndpoints;
    ESP8266WebServer* server = nullptr;
    IPAddress localIp;
    IPAddress gateway;
    IPAddress subnet;
};


#endif
