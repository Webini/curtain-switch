#include "NormalSTA.h"

NormalSTA::NormalSTA(HardManager* _hardman, AbstractSensor* _sensor) : sensor(_sensor), hardman(_hardman) {}

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

  if (this->timeClient && millis() - this->lastTimeUpdateAt > REFRESH_NTP_TIME_INTERVAL) {
    log_printf("[NormalSTA::loop] NTP update");
    this->timeClient->update();
    this->lastTimeUpdateAt = millis();
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
  this->ntpUDP = new WiFiUDP();
  this->timeClient = new NTPClient(*this->ntpUDP, NTP_SERVER_ADDRESS, TIME_ZONE_OFFSET, NTP_UPDATE_INTERVAL_MS);
  this->server = new ESP8266WebServer(ip, 80);
  this->server->onNotFound(std::bind(&NormalSTA::onNotFound, this));
  this->server->on("/", std::bind(&NormalSTA::onHomePage, this));
  this->server->on("/open", std::bind(&NormalSTA::onOpenPage, this));
  this->server->on("/close", std::bind(&NormalSTA::onClosePage, this));
  this->server->on("/stop", std::bind(&NormalSTA::onStopPage, this));
  this->server->on("/prometheus", std::bind(&NormalSTA::onPrometheusPage, this));
  this->httpUpdater = new ESP8266HTTPUpdateServer;
  this->httpUpdater->setup(this->server);
  this->timeClient->begin();
  this->timeClient->update();
  this->lastTimeUpdateAt = millis();
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

void NormalSTA::onPrometheusPage() {
  log_printf("[NormalSTA::onPrometheusPage]");

  if (!this->timeClient) {
    this->server->send(503, "text/plain", "");
    return;
  }
  
  char buff[300] = {0};
  float curtainPosition = 0;
  unsigned long long time = 0;
  
  if (this->timeClient) {
    time = this->timeClient->getMsEpochTime();
  }

  if (this->hardman) {
    curtainPosition = this->hardman->getCurrentPositionValue();
  }

  if (this->sensor) {
    double temperature = this->sensor->getTemperature();
    double pressure = this->sensor->getPressure();
    double altitude = this->sensor->getAltitude();
    snprintf(
      buff, 
      400, 
      "iot_temperature %f %llu\n" // temp °C
      "iot_pressure %f %llu\n" //  press hPa
      "iot_altitude %f %llu\n" // alt meter
      "iot_curtain_position %f %llu\n", // curtain position
      temperature, time,
      pressure, time,
      altitude, time,
      curtainPosition, time
    );
    this->server->send(200, "text/plain", buff);
    return;
  }
  
  snprintf(
    buff, 
    400,
    "iot_curtain_position %f %llu\n", // curtain position
    curtainPosition, time
  );
  this->server->send(200, "text/plain", buff);
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

  if (this->timeClient) {
    delete this->timeClient;
    this->timeClient = nullptr;
  }

  if (this->ntpUDP) {
    delete this->ntpUDP;
    this->ntpUDP = nullptr;
  }
}
