#ifndef CONFIGURATION_ENDPOINTS_H
#define CONFIGURATION_ENDPOINTS_H

#include <ESP8266WebServer.h>
#include <functional>
#include "log.h"

class ConfigurationEndpoints {
  public:
    typedef std::function<void(const char*, const char*, const char*)> WifiCredentialsDefinedCallbackFunction;
    ConfigurationEndpoints();
    void begin(ESP8266WebServer* server);
    void onHomePage();
    void onConfigurationSent();
    void onWifiCredentialsDefined(WifiCredentialsDefinedCallbackFunction callback);

  private:
    ESP8266WebServer* server;
    WifiCredentialsDefinedCallbackFunction callback = nullptr;
};

#endif
