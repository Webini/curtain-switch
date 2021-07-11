#ifndef NORMAL_AP_H
#define NORMAL_AP_H

#include <ESP8266WebServer.h>
#include <functional>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiUdp.h>
#include "NTPClient.h"
#include "ConfigurationEndpoints.h"
#include "HardManager.h"
#include "SleepMonitor.h"
#include "AbstractSensor.h"
#include "log.h"

#define NTP_SERVER_ADDRESS "europe.pool.ntp.org"
#define NTP_UPDATE_INTERVAL_MS 1800000
#define TIME_ZONE_OFFSET 0

#define REFRESH_NTP_TIME_INTERVAL 24 * 60 * 60000 // ms

class NormalSTA {
  public:
    typedef std::function<void(int)> WifiConnectionStatusCallbackFunction;
    
    NormalSTA(HardManager* hardman, SleepMonitor* sleepMonitor, AbstractSensor* sensor);
    ~NormalSTA();
    
    void begin(const char* ssid, const char* password, const char* name);
    void loop();
    void setConfiguration(const char* ssid, const char* password, const char* serverUrl);
    void onWifiConnectionFailed(WifiConnectionStatusCallbackFunction callback);
    void onWifiConnectionSuccess(WifiConnectionStatusCallbackFunction callback);
    void onConfigurationDefined(ConfigurationEndpoints::ConfigurationDefinedCallbackFunction callback);
    void onClosePage();
    void onOpenPage();
    void onStopPage();
    void onHomePage();
    void onInfoPage();
    void onRebootPage();
    void onPrometheusPage();
    void onNotFound();

  private:
    void clean();
    void onConnected();
    SleepMonitor* sleepMonitor = nullptr;
    ConfigurationEndpoints configurationEndpoints;
    ESP8266WebServer* server = nullptr;
    ESP8266HTTPUpdateServer* httpUpdater = nullptr;
    HardManager* hardman = nullptr;
    const char* ssid = "";
    const char* password = "";
    const char* name = "curtain switch";
    unsigned long lastConnectingCheck = 0;
    WifiConnectionStatusCallbackFunction wifiErrorCallback = nullptr;
    WifiConnectionStatusCallbackFunction wifiSuccessCallback = nullptr;
    AbstractSensor* sensor = nullptr;
    WiFiUDP* ntpUDP = nullptr;
    NTPClient* timeClient = nullptr;
    unsigned int lastTimeUpdateAt = 0;
};

#endif
