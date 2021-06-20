#ifndef NORMAL_AP_H
#define NORMAL_AP_H

//include <functional>
#include <ESP8266WiFi.h>
#include "HardManager.h"
#include "log.h"

class NormalAP {
  public:
    NormalAP(HardManager* hardman);
    ~NormalAP();
    typedef std::function<void(int)> WifiConnectionStatusCallbackFunction;
    void begin(const char* ssid, const char* password, const char* serverUrl);
    void loop();
    void setConfiguration(const char* ssid, const char* password, const char* serverUrl);
    void onWifiConnectionFailed(WifiConnectionStatusCallbackFunction callback);
    void onWifiConnectionSuccess(WifiConnectionStatusCallbackFunction callback);

  private:
    void onConnected();
    HardManager* hardman = nullptr;
    const char* ssid = "";
    const char* password = "";
    const char* serverUrl = "";
    unsigned long lastConnectingCheck = 0;
    WifiConnectionStatusCallbackFunction wifiErrorCallback = nullptr;
    WifiConnectionStatusCallbackFunction wifiSuccessCallback = nullptr;
};

#endif
