#include "ConfigurationEndpoints.h"

ConfigurationEndpoints::ConfigurationEndpoints() {
}


void ConfigurationEndpoints::begin(ESP8266WebServer* server, const char* path) {
  log_printf("[ConfigurationEndpoints::begin]");
  this->server = server;
  this->server->on(path, std::bind(&ConfigurationEndpoints::onHomePage, this));
  this->server->on("/configuration/save", HTTP_POST, std::bind(&ConfigurationEndpoints::onConfigurationSent, this));
}


void ConfigurationEndpoints::onHomePage() {
  log_printf("[ConfigurationEndpoints::onHomePage]");
  this->server->send(200, "text/html", R"<<<EOF(
  <html>
    <body>
      <form autocomplete="off" method="POST" action="/configuration/save">
        <p>
          <label for="ssid">SSID</label>
          <input type="text" value="" name="ssid" id="ssid" maxlength="34" required />
        </p>
        <p>
          <label for="password">Wifi Password</label>
          <input type="text" value="" name="password" id="password" required />
        </p>
        <p>
          <label for="name">Curtain name</label>
          <input type="text" value="" name="name" id="name" maxlength="80" required />
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
  String name = this->server->arg("name");
  log_printf("[ConfigurationEndpoints::onConfigurationSent]ssid: %s, password: %s, name: %s", ssid.c_str(), password.c_str(), name.c_str());
  this->server->send(200, "text/html", "<h1>Configured</h1>");
  
  if (this->callback) {
    this->callback(ssid.c_str(), password.c_str(), name.c_str());
  }
}


void ConfigurationEndpoints::onConfigurationDefined(ConfigurationDefinedCallbackFunction callback) {
  this->callback = callback;
}
