#pragma once
#include <lvgl.h>

void init_touch(int width, int height, int rotation);
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
