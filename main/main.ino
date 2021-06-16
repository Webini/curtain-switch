#include <EEPROM.h>
#include "GlobalConfiguration.h"
#include "Commander.h"
#include "HardManager.h"

GlobalConfiguration conf;
Commander com(&conf);
HardManager hardman;

void onCloseBtn(bool down, unsigned long since, HardManager* hm) {
  Serial.print("New close btn state : ");
  Serial.print(down);
  Serial.print(", since ");
  Serial.print(since);
  Serial.println();
  hm->close(50);
}
void onStopBtn(bool down, unsigned long since, HardManager* hm) {
  unsigned long now = millis();
  Serial.print("New stop btn state : ");
  Serial.print(down);
  Serial.print(", since ");
  Serial.print(since);
  Serial.print(", now ");
  Serial.print(now);
  Serial.print(", diff ");
  Serial.print(now - since);
  Serial.println();
  hm->stop();
  if (!down && now - since > 500) {
    hm->setLedMode(now - since > 5000 ? HardManager::LedMode::OFF : HardManager::LedMode::BLINK);
  }
}
void onOpenBtn(bool down, unsigned long since, HardManager* hm) {
  Serial.print("New open btn state : ");
  Serial.print(down);
  Serial.print(", since ");
  Serial.print(since);
  Serial.println();
  hm->open();
}

void onUpRelay(bool on, unsigned long since, HardManager* hm) {
  Serial.print("New up relay state : ");
  Serial.print(on);
  Serial.print(", since ");
  Serial.print(since);
  Serial.println();
}

void onDownRelay(bool on, unsigned long since, HardManager* hm) {
  Serial.print("New down relay state : ");
  Serial.print(on);
  Serial.print(", since ");
  Serial.print(since);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  conf.begin();
  
  Serial.println(conf.isConfigured() ? "Configured" : "Not configured");
  Serial.print("Wifi ssid : ");
  Serial.println(conf.getWifiSsid());
  Serial.print("Wifi password : ");
  Serial.println(conf.getWifiPassword());
  Serial.print("Server url : ");
  Serial.println(conf.getServerUrl());

  hardman.onCloseBtnStateChange(onCloseBtn);
  hardman.onStopBtnStateChange(onStopBtn);
  hardman.onOpenBtnStateChange(onOpenBtn);
  hardman.onUpRelayStateChange(onUpRelay);
  hardman.onDownRelayStateChange(onDownRelay);
  hardman.begin();
  
  com.begin();
}

void loop() {
  hardman.loop();
  com.loop();
  delay(50);
}
