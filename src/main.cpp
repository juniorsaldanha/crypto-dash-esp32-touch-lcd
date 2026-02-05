#include "api/crypto.h"
#include "core/config.h"
#include "core/settings.h"
#include "core/types.h"
#include "network/ntp.h"
#include "network/web_server.h"
#include "network/wifi_manager.h"
#include "ui/display.h"
#include "ui/touch.h"
#include "ui/ui_manager.h"
#include <Arduino.h>
#include <lvgl.h>

// Global Variables
QueueHandle_t crypto_queue;
SemaphoreHandle_t fetch_sem;
// Buffers for LVGL Direct Mode
uint32_t bufSize;
lv_disp_draw_buf_t draw_buf;
lv_color_t *disp_draw_buf;
lv_disp_drv_t disp_drv;

void gui_task(void *parameter) {
  while (true) {
    lv_tick_inc(5);
    lv_timer_handler();

    CryptoData data;
    if (xQueueReceive(crypto_queue, &data, 0) == pdTRUE) {
      Serial.printf("UI Task received: %s $%.2f\n", data.symbol,
                    data.price_usd);

      // Save to cache based on index (Hack: matching symbol to index logic
      // again or just rely on arrival?) We need the index. Let's look it up
      // again like in screen_home or just update all? Better: ui_manager
      // handles the update, let's let ui_manager OR settings handle the
      // mapping. But main.cpp doesn't know the index easily without checking
      // settings. Let's do a quick lookup.
      AppSettings settings = get_settings();
      for (int i = 0; i < 3; i++) {
        if (strcmp(data.symbol, settings.crypto_symbols[i]) == 0) {
          save_crypto_cache(i, data);
          break;
        }
      }

      update_data_on_ui(data);
    }

    update_ui_tasks(); // Handles time and stale checks
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

void api_task(void *parameter) {
  // Fast Fetch: Wait for WiFi connection then fetch immediately
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(pdMS_TO_TICKS(500));
  }
  vTaskDelay(
      pdMS_TO_TICKS(1000)); // Short grace period for network stabilization

  while (true) {
    fetch_crypto_data(crypto_queue);
    // Wait for 5 minutes OR signal from settings
    xSemaphoreTake(fetch_sem, pdMS_TO_TICKS(300000));
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32-S3 Crypto Monitor - Refactored");

  init_settings(); // Load settings first

  init_display();
  init_touch(LCD_H_RES, LCD_V_RES, LCD_ROTATION);
  lv_init();

  // Buffer Allocation
  bufSize = LCD_H_RES * LCD_V_RES;
  disp_draw_buf = (lv_color_t *)heap_caps_malloc(
      bufSize * 2, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  if (!disp_draw_buf)
    disp_draw_buf =
        (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_8BIT);

  if (!disp_draw_buf) {
    Serial.println("LVGL disp_draw_buf allocate failed!");
  } else {
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, bufSize);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_H_RES;
    disp_drv.ver_res = LCD_V_RES;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    // disp_drv.direct_mode = true; // Disabled for partial refresh
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    init_ui();
  }

  init_wifi();
  init_ntp(); // Start NTP sync
  init_web_server();

  init_web_server();

  crypto_queue = xQueueCreate(20, sizeof(CryptoData));
  fetch_sem = xSemaphoreCreateBinary();
  xTaskCreatePinnedToCore(api_task, "api", 16384, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(gui_task, "gui", 8192, NULL, 2, NULL, 0);
}

void loop() {
  handle_web_server();
  vTaskDelay(pdMS_TO_TICKS(10));
}
