#include "NormalSTA.h"

NormalSTA::NormalSTA(HardManager* _hardman, SleepMonitor* _sleepMonitor, AbstractSensor* _sensor) : sensor(_sensor), sleepMonitor(_sleepMonitor), hardman(_hardman) {}

NormalSTA::~NormalSTA() {
  this->clean();
  auto result = WiFi.disconnect(true);
  log_printf("[ConfigurationAP::~ConfigurationAP]Disconnect : %d", result);
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

void NormalSTA::begin(const char* ssid, const char* password, const char* name) {
  log_printf("[ConfigurationAP::begin]ssid: %s, password: %s", ssid, password);
  this->setConfiguration(ssid, password, name);

  if (name && strlen(name) > 0) {
    WiFi.hostname(name);
    log_printf("[ConfigurationAP::begin]Set hostname: %s", name);
  } else {
    char hostname[34] = {0};
    unsigned int chipId = ESP.getChipId();
    snprintf(hostname, 33, "%s%08X", AP_SSID_PREFIX, chipId);
    WiFi.hostname(hostname);
    log_printf("[ConfigurationAP::begin]Set hostname: %s", hostname);
  }
  
  auto result = WiFi.enableSTA(true);
  log_printf(
    "[NormalSTA::begin]WiFi Enable STA... %s", 
    result ? "success" : "failed"
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
  this->server->on("/reboot", std::bind(&NormalSTA::onRebootPage, this));
  this->server->on("/prometheus", std::bind(&NormalSTA::onPrometheusPage, this));
  this->server->on("/info", std::bind(&NormalSTA::onInfoPage, this));
  this->configurationEndpoints.begin(this->server, "/configuration");
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


void NormalSTA::setConfiguration(const char* ssid, const char* password, const char* name) {
  this->ssid = ssid;
  this->password = password;
  this->name = name;
}


void NormalSTA::onNotFound() {
  log_printf("[NormalSTA::onNotFound]");
  this->server->send(404, "text/html", "<h1>Not found :)</h1>");
}


void NormalSTA::onRebootPage() {
  this->server->send(200, "text/html", "<html><body>ok, <a href=\"/\">back</a></body></html>");
  ESP.restart();
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
  this->server->send(200, "text/html", "<html><body><a href=\"/open\">open</a> <a href=\"/close\">close</a> <a href=\"/stop\">stop</a> <a href=\"/configuration\">config</a></body></html>");
}


void NormalSTA::onPrometheusPage() {
  log_printf("[NormalSTA::onPrometheusPage]");

  if (!this->timeClient) {
    this->server->send(503, "text/plain", "");
    return;
  }
  
  char buff[1000] = {0};
  float curtainPosition = 0;
  unsigned long long time = 0;
  int freeHeap = ESP.getFreeHeap();
  int headFragmentation = ESP.getHeapFragmentation();
  
  if (this->timeClient) {
    time = this->timeClient->getMsEpochTime();
  }

  if (this->hardman) {
    curtainPosition = this->hardman->getCurrentPositionValue();
  }

  if (!time) {
    this->server->send(503, "text/plain", "");
    return;
  }

  if (this->sensor->isDetected()) {
    double temperature = this->sensor->getTemperature();
    double pressure = this->sensor->getPressure();
    double altitude = this->sensor->getAltitude();
    snprintf(
      buff, 
      1000, 
      "iot_temperature{name=\"%s\"} %f %llu\n" // temp °C
      "iot_pressure{name=\"%s\"} %f %llu\n" //  press hPa
      "iot_altitude{name=\"%s\"} %f %llu\n" // alt meter
      "iot_curtain_position{name=\"%s\"} %f %llu\n" // curtain position
      "iot_free_heap{name=\"%s\"} %i %llu\n"
      "iot_heap_fragmentation{name=\"%s\"} %i %llu\n"
      "iot_sleep_since{name=\"%s\"} %u %llu\n",
      this->name, temperature, time,
      this->name, pressure, time,
      this->name, altitude, time,
      this->name, curtainPosition, time,
      this->name, freeHeap, time,
      this->name, headFragmentation, time,
      this->name, this->sleepMonitor->getElapsedTimeSinceLastActivity(), time
    );
    this->server->send(200, "text/plain", buff);
    return;
  }
  
  snprintf(
    buff, 
    1000,
    "iot_curtain_position{name=\"%s\"} %f %llu\n" // curtain position
    "iot_free_heap{name=\"%s\"} %i %llu\n"
    "iot_heap_fragmentation{name=\"%s\"} %i %llu\n"
    "iot_sleep_since{name=\"%s\"} %u %llu\n",
    this->name, curtainPosition, time,
    this->name, freeHeap, time,
    this->name, headFragmentation, time,
    this->name, this->sleepMonitor->getElapsedTimeSinceLastActivity(), time
  );
  this->server->send(200, "text/plain", buff);
}


void NormalSTA::onInfoPage() {
  char buff[400] = {0};
  
  snprintf(
    buff, 
    400,
    "version : %s\n"
    "sensor detected : %s\n"
    "elapsed time since last activity: %u\n"
    "ip : %s\n"
    "sensor addr : 0x%02X\n"
    "debug : %s\n"
    "name : %s\n",
    VERSION,
    this->sensor->isDetected() ? "yes" : "no",
    this->sleepMonitor->getElapsedTimeSinceLastActivity(),
    WiFi.localIP().toString().c_str(),
    this->sensor->getAddress(),
    #ifdef DEBUG
    "yes",
    #else
    "no",
    #endif
    this->name
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


void NormalSTA::onConfigurationDefined(ConfigurationEndpoints::ConfigurationDefinedCallbackFunction callback) {
  this->configurationEndpoints.onConfigurationDefined(callback);
}
