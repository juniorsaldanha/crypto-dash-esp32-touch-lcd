#pragma once
#include <Arduino.h>

void init_wifi();
void wifi_reset_settings();
void wifi_connect(const char *ssid, const char *pass);
