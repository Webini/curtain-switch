#include "NormalAP.h"

NormalAP::NormalAP(HardManager* _hardman) : hardman(_hardman) {}

NormalAP::~NormalAP() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
}

void NormalAP::loop() {
  if (this->lastConnectingCheck && millis() - this->lastConnectingCheck > 500) {
    int status = WiFi.status();
    if (status == WL_IDLE_STATUS || status == WL_DISCONNECTED) {
      log_printf("[NormalAP::loop]Connecting... (%d)", status);
      this->lastConnectingCheck = millis();
      return;
    }

    this->lastConnectingCheck = 0;
    if (status == WL_CONNECTED) {
      log_printf("[NormalAP::loop]Connected !");
      this->onConnected();
      return;  
    }

    if (this->wifiErrorCallback) {
      log_printf("[NormalAP::loop]Connection error (%d)", status);
      this->wifiErrorCallback(status);
      return;
    }
  }
}


void NormalAP::begin(const char* ssid, const char* password, const char* serverUrl) {
  log_printf("[ConfigurationAP::begin]Ssid: %s, password: %s, serverUrl: %s", ssid, password, serverUrl);
  this->setConfiguration(ssid, password, serverUrl);

  log_printf(
    "[NormalAP::begin]Wifi mode set to WIFI_STA... %s", 
    WiFi.mode(WIFI_STA) ? "success" : "failed"
  );
  
  log_printf("[NormalAP::begin]Starting connection...");
  WiFi.begin(ssid, password);

  this->lastConnectingCheck = millis();
}


void NormalAP::onConnected() {
  IPAddress ip = WiFi.localIP();
  log_printf("[NormalAP::onConnected]Connected with ip %s", ip.toString().c_str());
  
  if (this->wifiSuccessCallback) {
    this->wifiSuccessCallback(WL_CONNECTED);
  }
  
  // @todo start server
}


void NormalAP::onWifiConnectionFailed(WifiConnectionStatusCallbackFunction callback) {
  this->wifiErrorCallback = callback;
}


void NormalAP::onWifiConnectionSuccess(WifiConnectionStatusCallbackFunction callback) {
  this->wifiSuccessCallback = callback;
}


void NormalAP::setConfiguration(const char* ssid, const char* password, const char* serverUrl) {
  this->ssid = ssid;
  this->password = password;
  this->serverUrl = serverUrl;
}
