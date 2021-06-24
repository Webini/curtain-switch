#include "ConfigurationEndpoints.h"

ConfigurationEndpoints::ConfigurationEndpoints() {
}


void ConfigurationEndpoints::begin(ESP8266WebServer* server) {
  log_printf("[ConfigurationEndpoints::begin]");
  this->server = server;
  this->server->on("/", std::bind(&ConfigurationEndpoints::onHomePage, this));
  this->server->on("/save", HTTP_POST, std::bind(&ConfigurationEndpoints::onConfigurationSent, this));
}


void ConfigurationEndpoints::onHomePage() {
  log_printf("[ConfigurationEndpoints::onHomePage]");
  this->server->send(200, "text/html", R"<<<EOF(
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


void ConfigurationEndpoints::onConfigurationSent() {
  String ssid = this->server->arg("ssid");
  String password = this->server->arg("password");
  String serverUrl = this->server->arg("serverUrl");
  log_printf("[ConfigurationEndpoints::onConfigurationSent]ssid: %s, password: %s, serverUrl: %s", ssid.c_str(), password.c_str(), serverUrl.c_str());
  this->server->send(200, "text/html", "<h1>Configured</h1>");
  
  if (this->callback) {
    this->callback(ssid.c_str(), password.c_str(), serverUrl.c_str());
  }
}


void ConfigurationEndpoints::onWifiCredentialsDefined(WifiCredentialsDefinedCallbackFunction callback) {
  this->callback = callback;
}
