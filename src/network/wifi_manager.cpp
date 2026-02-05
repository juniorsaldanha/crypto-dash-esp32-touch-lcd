#include "wifi_manager.h"
#include <WiFiManager.h>

void init_wifi() {
  WiFiManager wm;
  // wm.setDebugOutput(false);
  if (!wm.autoConnect("CryptoMonitor-AP")) {
    Serial.println("WiFi failed");
    ESP.restart();
  }
}

void wifi_reset_settings() {
  WiFiManager wm;
  wm.resetSettings();
  WiFi.disconnect(true, true); // Clear ESP32 NVS
  Serial.println("WiFi Credentials Reset");
  ESP.restart();
}

void wifi_connect(const char *ssid, const char *pass) {
  Serial.printf("Connecting to %s...\n", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  // We don't block here, let the system handle connection in background
  // The API task waits for WL_CONNECTED anyway.
}
