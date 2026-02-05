#include "settings.h"
#include <Preferences.h>

Preferences prefs;
AppSettings currentSettings;

void init_settings() {
  prefs.begin("settings", false);

  // Load Currency
  String curr = prefs.getString("currency", "usd");
  strlcpy(currentSettings.currency, curr.c_str(),
          sizeof(currentSettings.currency));

  // Load Cryptos (Keys changed to force defaults)
  // Default 0: bitcoin / BTC
  String id0 = prefs.getString("c_id0", "bitcoin");
  String sym0 = prefs.getString("c_sym0", "BTC");
  strlcpy(currentSettings.crypto_ids[0], id0.c_str(),
          sizeof(currentSettings.crypto_ids[0]));
  strlcpy(currentSettings.crypto_symbols[0], sym0.c_str(),
          sizeof(currentSettings.crypto_symbols[0]));

  // Default 1: ethereum / ETH
  String id1 = prefs.getString("c_id1", "ethereum");
  String sym1 = prefs.getString("c_sym1", "ETH");
  strlcpy(currentSettings.crypto_ids[1], id1.c_str(),
          sizeof(currentSettings.crypto_ids[1]));
  strlcpy(currentSettings.crypto_symbols[1], sym1.c_str(),
          sizeof(currentSettings.crypto_symbols[1]));

  // Default 2: solana / SOL
  String id2 = prefs.getString("c_id2", "solana");
  String sym2 = prefs.getString("c_sym2", "SOL");
  strlcpy(currentSettings.crypto_ids[2], id2.c_str(),
          sizeof(currentSettings.crypto_ids[2]));
  strlcpy(currentSettings.crypto_symbols[2], sym2.c_str(),
          sizeof(currentSettings.crypto_symbols[2]));

  prefs.end();
}

AppSettings get_settings() { return currentSettings; }

void save_settings(const AppSettings &settings) {
  currentSettings = settings;
  prefs.begin("settings", false);

  prefs.putString("currency", settings.currency);

  prefs.putString("c_id0", settings.crypto_ids[0]);
  prefs.putString("c_sym0", settings.crypto_symbols[0]);

  prefs.putString("c_id1", settings.crypto_ids[1]);
  prefs.putString("c_sym1", settings.crypto_symbols[1]);

  prefs.putString("c_id2", settings.crypto_ids[2]);
  prefs.putString("c_sym2", settings.crypto_symbols[2]);

  prefs.end();
}

void save_crypto_cache(int index, const CryptoData &data) {
  if (index < 0 || index >= 3)
    return;
  Preferences prefs;
  prefs.begin("crypto_cache", false); // New namespace
  char key[16];
  snprintf(key, sizeof(key), "data%d", index);
  prefs.putBytes(key, &data, sizeof(CryptoData));
  prefs.end();
}

bool load_crypto_cache(int index, CryptoData *data) {
  if (index < 0 || index >= 3)
    return false;
  Preferences prefs;
  prefs.begin("crypto_cache", true); // Read-only
  char key[16];
  snprintf(key, sizeof(key), "data%d", index);
  size_t len = prefs.getBytesLength(key);
  if (len == sizeof(CryptoData)) {
    prefs.getBytes(key, data, sizeof(CryptoData));
    prefs.end();
    return true;
  }
  prefs.end();
  return false;
}
