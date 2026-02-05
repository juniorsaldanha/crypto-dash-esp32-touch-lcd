#include "touch.h"
#include "../core/config.h"
#include "bsp_cst816.h"
#include <Wire.h>

void init_touch(int width, int height, int rotation) {
  Wire.begin(PIN_TP_SDA, PIN_TP_SCL);
  bsp_touch_init(&Wire, rotation, width, height);
}

void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
  uint16_t touchpad_x;
  uint16_t touchpad_y;
  bsp_touch_read();
  if (bsp_touch_get_coordinates(&touchpad_x, &touchpad_y)) {
    data->point.x = touchpad_x;
    data->point.y = touchpad_y;
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}
