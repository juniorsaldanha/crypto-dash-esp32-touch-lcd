#pragma once
#include "../../core/types.h"
#include <lvgl.h>

// Screen Init Functions
void init_screen_home(lv_obj_t *parent);
void init_screen_network(lv_obj_t *parent);
void init_screen_settings(lv_obj_t *parent);
void init_screen_wifi_config(lv_obj_t *parent);

// Update commands
void update_home_screen(const CryptoData &data);
void check_home_stale_data();
void update_time_label();
