#include "web_server.h"
#include <Preferences.h>
#include <WiFi.h>

WebServer server(80);

void init_web_server() {
  Preferences prefs;
  prefs.begin("crypto-monitor");
  // prefs.end(); // Nothing to load for now

  server.on("/", HTTP_GET, []() {
    String html =
        "<!DOCTYPE html><html><head><title>Crypto Monitor Admin</title>"
        "<style>body { font-family: sans-serif; max-width: 500px; margin: 50px "
        "auto; padding: 20px; background: #1e1e1e; color: white; "
        "}</style></head><body>"
        "<h1>ESP32-S3 Crypto Monitor</h1>"
        "<p>IP: " +
        WiFi.localIP().toString() +
        "</p>"
        // Removed API Key section
        "</body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/save", HTTP_POST,
            []() { server.send(200, "text/plain", "No settings to save."); });

  server.begin();
}

void handle_web_server() { server.handleClient(); }
