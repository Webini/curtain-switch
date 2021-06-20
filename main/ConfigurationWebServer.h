#ifndef CONFIGURATION_WEB_SERVER_H
#define CONFIGURATION_WEB_SERVER_H

#include <ESP8266WebServer.h>
#include "log.h"

class ConfigurationWebServer {
  public:
    typedef std::function<void(const char*, const char*, const char*)> WifiCredentialsDefinedCallbackFunction;
    ConfigurationWebServer(IPAddress addr);
    void onHomePage();
    void onConfigurationSent();
    void onNotFound();
    void begin();
    void loop();
    void onWifiCredentialsDefined(WifiCredentialsDefinedCallbackFunction callback);

  private:
    ESP8266WebServer server;
    WifiCredentialsDefinedCallbackFunction callback = nullptr;
};

#endif
