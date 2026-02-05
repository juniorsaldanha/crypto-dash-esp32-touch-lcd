#pragma once
#include "types.h"

void init_settings();
AppSettings get_settings();
void save_settings(const AppSettings &settings);
void save_crypto_cache(int index, const CryptoData &data);
bool load_crypto_cache(int index, CryptoData *data);
