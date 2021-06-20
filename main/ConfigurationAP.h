#ifndef CONFIGURATION_AP_H
#define CONFIGURATION_AP_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "ConfigurationWebServer.h"
#include "parameters.h"
#include "log.h"

class ConfigurationAP {
  public:
    ConfigurationAP();
    ~ConfigurationAP();
    void begin();
    void loop();
    void onWifiCredentialsDefined(ConfigurationWebServer::WifiCredentialsDefinedCallbackFunction callback);
    
  private:
    char ssid[34] = {0};
    ConfigurationWebServer server;
    IPAddress localIp;
    IPAddress gateway;
    IPAddress subnet;
};


#endif
