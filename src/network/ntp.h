#pragma once
#include <Arduino.h>

void init_ntp();
String get_current_time();
String get_current_date();
bool is_time_synced();
