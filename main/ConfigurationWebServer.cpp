#include "ConfigurationWebServer.h"

ConfigurationWebServer::ConfigurationWebServer(IPAddress addr) : server(addr, 80) {
  log_printf("[ConfigurationWebServer::ConfigurationWebServer]");
  this->server.on("/", std::bind(&ConfigurationWebServer::onHomePage, this));
  this->server.on("/save", HTTP_POST, std::bind(&ConfigurationWebServer::onConfigurationSent, this));
  this->server.onNotFound(std::bind(&ConfigurationWebServer::onNotFound, this));
}


void ConfigurationWebServer::onHomePage() {
  log_printf("[ConfigurationWebServer::onHomePage]");
  this->server.send(200, "text/html", R"<<<EOF(
  <html>
    <body>
      <form autocomplete="off" method="POST" action="/save">
        <p>
          <label for="ssid">SSID</label>
          <input type="text" value="" name="ssid" id="ssid" required />
        </p>
        <p>
          <label for="password">Wifi Password</label>
          <input type="text" value="" name="password" id="password" required />
        </p>
        <p>
          <label for="serverUrl">Server url</label>
          <input type="text" value="" name="serverUrl" id="serverUrl" />
        </p>
        <button type="submit">Submit</button>
      </form>
    </body>
  </html>
  )<<<EOF");
}

void ConfigurationWebServer::onConfigurationSent() {
  String ssid = this->server.arg("ssid");
  String password = this->server.arg("password");
  String serverUrl = this->server.arg("serverUrl");
  log_printf("[ConfigurationWebServer::onConfigurationSent]ssid: %s, password: %s, serverUrl: %s", ssid.c_str(), password.c_str(), serverUrl.c_str());
  this->server.send(200, "text/html", "<h1>Configured</h1>");
  
  if (this->callback) {
    this->callback(ssid.c_str(), password.c_str(), serverUrl.c_str());
  }
}


void ConfigurationWebServer::onNotFound() {
  log_printf("[ConfigurationWebServer::onNotFound]");
  this->server.send(404, "text/html", "<h1>Not found :(</h1>");
}


void ConfigurationWebServer::begin() {
  log_printf("[ConfigurationWebServer::begin]");
  this->server.begin();
}


void ConfigurationWebServer::loop() {
  this->server.handleClient();
}


void ConfigurationWebServer::onWifiCredentialsDefined(WifiCredentialsDefinedCallbackFunction callback) {
  this->callback = callback;
}
