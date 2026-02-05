#pragma once
#include <Arduino.h>

struct CryptoData {
  char symbol[16];
  float price_usd;
  float change_usd;
  float price_eur;
  float change_eur;
  float price_brl;
  float change_brl;
  float price_gbp;
  float change_gbp;
  unsigned long timestamp;
};

struct AppSettings {
  char currency[4];          // usd, eur, brl
  char crypto_ids[3][32];    // bitcoin, ethereum, solana
  char crypto_symbols[3][8]; // BTC, ETH, SOL
};
