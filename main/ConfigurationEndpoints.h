#ifndef CONFIGURATION_ENDPOINTS_H
#define CONFIGURATION_ENDPOINTS_H

#include <ESP8266WebServer.h>
#include <functional>
#include "log.h"

class ConfigurationEndpoints {
  public:
    typedef std::function<void(const char*, const char*, const char*)> ConfigurationDefinedCallbackFunction;
    ConfigurationEndpoints();
    void begin(ESP8266WebServer* server, const char* path = "/");
    void onHomePage();
    void onConfigurationSent();
    void onConfigurationDefined(ConfigurationDefinedCallbackFunction callback);

  private:
    ESP8266WebServer* server;
    ConfigurationDefinedCallbackFunction callback = nullptr;
};

#endif
