#include "crypto.h"
#include "../core/settings.h"
#include "../core/types.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

void fetch_crypto_data(QueueHandle_t dataQueue) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping fetch");
    return;
  }

  AppSettings settings = get_settings();

  // Construct URL dynamically
  String url = "https://api.coingecko.com/api/v3/simple/price?ids=";
  url += settings.crypto_ids[0];
  url += ",";
  url += settings.crypto_ids[1];
  url += ",";
  url += settings.crypto_ids[2];
  url += "&vs_currencies=usd,eur,brl,gbp";
  url += "&include_24hr_change=true";

  Serial.printf("Fetching: %s\n", url.c_str());

  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(10);

  HTTPClient http;
  if (!http.begin(client, url)) {
    Serial.println("HTTP begin failed");
    return;
  }

  int httpCode = http.GET();
  Serial.printf("HTTP GET code: %d\n", httpCode);

  if (httpCode == 200) {
    String payload = http.getString();
    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.printf("deserializeJson() failed: %s\n", error.c_str());
      return;
    }

    for (int i = 0; i < 3; i++) {
      CryptoData data;
      const char *id = settings.crypto_ids[i];

      // Safety check if ID exists in response
      if (!doc.containsKey(id))
        continue;

      strlcpy(data.symbol, settings.crypto_symbols[i], sizeof(data.symbol));
      data.timestamp = millis();

      // USD
      data.price_usd = doc[id]["usd"].as<float>();
      data.change_usd = doc[id]["usd_24h_change"].as<float>();

      // EUR
      data.price_eur = doc[id]["eur"].as<float>();
      data.change_eur = doc[id]["eur_24h_change"].as<float>();

      // BRL
      data.price_brl = doc[id]["brl"].as<float>();
      data.change_brl = doc[id]["brl_24h_change"].as<float>();

      // GBP
      data.price_gbp = doc[id]["gbp"].as<float>();
      data.change_gbp = doc[id]["gbp_24h_change"].as<float>();

      xQueueSend(dataQueue, &data, 0);
    }

  } else {
    Serial.printf("HTTP Error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}
