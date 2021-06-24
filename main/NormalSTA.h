#ifndef NORMAL_AP_H
#define NORMAL_AP_H

#include <ESP8266WebServer.h>
#include <functional>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "ConfigurationEndpoints.h"
#include "HardManager.h"
#include "log.h"

class NormalSTA {
  public:
    typedef std::function<void(int)> WifiConnectionStatusCallbackFunction;
    
    NormalSTA(HardManager* hardman);
    ~NormalSTA();
    
    void begin(const char* ssid, const char* password, const char* serverUrl);
    void loop();
    void setConfiguration(const char* ssid, const char* password, const char* serverUrl);
    void onWifiConnectionFailed(WifiConnectionStatusCallbackFunction callback);
    void onWifiConnectionSuccess(WifiConnectionStatusCallbackFunction callback);
    void onNotFound();

  private:
    void clean();
    void onConnected();
    ConfigurationEndpoints configurationEndpoints;
    ESP8266WebServer* server = nullptr;
    ESP8266HTTPUpdateServer* httpUpdater = nullptr;
    HardManager* hardman = nullptr;
    const char* ssid = "";
    const char* password = "";
    const char* serverUrl = "";
    unsigned long lastConnectingCheck = 0;
    WifiConnectionStatusCallbackFunction wifiErrorCallback = nullptr;
    WifiConnectionStatusCallbackFunction wifiSuccessCallback = nullptr;
};

#endif