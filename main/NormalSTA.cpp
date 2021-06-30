#include "NormalSTA.h"

NormalSTA::NormalSTA(HardManager* _hardman) : hardman(_hardman) {}

NormalSTA::~NormalSTA() {
  this->clean();
  log_printf("[ConfigurationAP::~ConfigurationAP]Disconnect : %d", WiFi.disconnect(true));
}

void NormalSTA::loop() {
  if (this->lastConnectingCheck && millis() - this->lastConnectingCheck > 500) {
    int status = WiFi.status();
    if (status == WL_IDLE_STATUS || status == WL_DISCONNECTED) {
      log_printf("[NormalSTA::loop]Connecting... (%d)", status);
      this->lastConnectingCheck = millis();
      return;
    }

    this->lastConnectingCheck = 0;
    if (status == WL_CONNECTED) {
      log_printf("[NormalSTA::loop]Connected !");
      this->onConnected();
      return;  
    }

    if (this->wifiErrorCallback) {
      log_printf("[NormalSTA::loop]Connection error (%d)", status);
      this->wifiErrorCallback(status);
      return;
    }
  }

  if (this->server) {
    this->server->handleClient();    
  }
}


void NormalSTA::begin(const char* ssid, const char* password, const char* serverUrl) {
  log_printf("[ConfigurationAP::begin]Ssid: %s, password: %s, serverUrl: %s", ssid, password, serverUrl);
  this->setConfiguration(ssid, password, serverUrl);

  log_printf(
    "[NormalSTA::begin]WiFi Enable STA... %s", 
    WiFi.enableSTA(true) ? "success" : "failed"
  );
  
  log_printf("[NormalSTA::begin]Starting connection...");
  WiFi.begin(ssid, password);

  this->lastConnectingCheck = millis();
}


void NormalSTA::onConnected() {
  IPAddress ip = WiFi.localIP();
  log_printf("[NormalSTA::onConnected]Connected with ip %s", ip.toString().c_str());
  
  if (this->wifiSuccessCallback) {
    this->wifiSuccessCallback(WL_CONNECTED);
  }

  
  this->clean();
  this->server = new ESP8266WebServer(ip, 80);
  this->server->onNotFound(std::bind(&NormalSTA::onNotFound, this));
  this->server->on("/", std::bind(&NormalSTA::onHomePage, this));
  this->server->on("/open", std::bind(&NormalSTA::onOpenPage, this));
  this->server->on("/close", std::bind(&NormalSTA::onClosePage, this));
  this->server->on("/stop", std::bind(&NormalSTA::onStopPage, this));
  this->httpUpdater = new ESP8266HTTPUpdateServer;
  this->httpUpdater->setup(this->server);
  this->server->begin();
}


void NormalSTA::onWifiConnectionFailed(WifiConnectionStatusCallbackFunction callback) {
  this->wifiErrorCallback = callback;
}


void NormalSTA::onWifiConnectionSuccess(WifiConnectionStatusCallbackFunction callback) {
  this->wifiSuccessCallback = callback;
}


void NormalSTA::setConfiguration(const char* ssid, const char* password, const char* serverUrl) {
  this->ssid = ssid;
  this->password = password;
  this->serverUrl = serverUrl;
}


void NormalSTA::onNotFound() {
  log_printf("[NormalSTA::onNotFound]");
  this->server->send(404, "text/html", "<h1>Not found :)</h1>");
}


void NormalSTA::onClosePage() {
  log_printf("[NormalSTA::onClosePage]");
  this->hardman->setPosition(POSITION_CLOSED);
  this->server->send(200, "text/html", "<html><body>ok, <a href=\"/\">back</a></body></html>");
}


void NormalSTA::onOpenPage() {
  log_printf("[NormalSTA::onOpenPage]");
  this->hardman->setPosition(POSITION_OPENED);
  this->server->send(200, "text/html", "<html><body>ok, <a href=\"/\">back</a></body></html>");
}


void NormalSTA::onStopPage() {
  log_printf("[NormalSTA::onStopPage]");
  this->hardman->stop();
  this->server->send(200, "text/html", "<html><body>ok, <a href=\"/\">back</a></body></html>");
}


void NormalSTA::onHomePage() {  
  log_printf("[NormalSTA::onHomePage]");
  this->server->send(200, "text/html", "<html><body><a href=\"/open\">open</a> <a href=\"/close\">close</a> <a href=\"/stop\">stop</a></body></html>");
}


void NormalSTA::clean() {
  if (this->httpUpdater) {
    delete httpUpdater;
    this->httpUpdater = nullptr;
  }

  if (this->server) {
    this->server->stop();
    delete this->server;
    this->server = nullptr;
  }
}
