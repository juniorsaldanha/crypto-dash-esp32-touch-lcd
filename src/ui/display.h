#pragma once
#include "../core/config.h"
#include <Arduino_GFX_Library.h>
#include <lvgl.h>

extern Arduino_GFX *gfx;

void init_display();
void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
                   lv_color_t *color_p);
