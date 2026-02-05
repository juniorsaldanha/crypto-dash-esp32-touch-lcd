#pragma once
#include "../core/types.h"
#include <lvgl.h>

void init_ui();
void load_screen(int index);
void update_ui_tasks();
void update_data_on_ui(const CryptoData &data);
