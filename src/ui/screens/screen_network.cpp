#include "../../network/ntp.h"
#include "../ui_manager.h"
#include "screens.h"
#include <WiFi.h>

static lv_obj_t *lbl_ip;
static lv_obj_t *lbl_ssid;
static lv_obj_t *lbl_rssi;
static lv_obj_t *lbl_mac;

static void btn_config_cb(lv_event_t *e) {
  load_screen(3); // Navigate to WiFi Config
}

void init_screen_network(lv_obj_t *parent) {
  lv_obj_t *cont = lv_obj_create(parent);
  lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
  lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(cont, 15, 0);
  lv_obj_set_style_bg_color(cont, lv_color_hex(0x0d1117), 0);
  lv_obj_set_style_border_width(cont, 0, 0);

  lv_obj_t *title = lv_label_create(cont);
  lv_label_set_text(title, "Network Info");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(title, lv_color_white(), 0);
  lv_obj_set_style_pad_bottom(title, 20, 0);

  // Helpers
  auto create_row = [&](const char *label, lv_obj_t **val_label_ptr) {
    lv_obj_t *row = lv_obj_create(cont);
    lv_obj_set_width(row, lv_pct(100));
    lv_obj_set_height(row, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(row, 0, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *l = lv_label_create(row);
    lv_label_set_text(l, label);
    lv_obj_set_style_text_color(l, lv_color_hex(0x8b949e), 0);

    *val_label_ptr = lv_label_create(row);
    lv_label_set_text(*val_label_ptr, "...");
    lv_obj_set_style_text_color(*val_label_ptr, lv_color_white(), 0);
  };

  create_row("IP Address", &lbl_ip);
  create_row("SSID", &lbl_ssid);
  create_row("Signal", &lbl_rssi);
  create_row("MAC", &lbl_mac);

  // Configure Button
  lv_obj_t *btn_cfg = lv_btn_create(cont);
  lv_obj_set_width(btn_cfg, lv_pct(100));
  lv_obj_set_style_pad_top(cont, 20, 0); // Add spacing before button
  lv_obj_set_style_bg_color(btn_cfg, lv_color_hex(0x30363d), 0);
  lv_obj_t *l_cfg = lv_label_create(btn_cfg);
  lv_label_set_text(l_cfg, "Configure WiFi");
  lv_obj_center(l_cfg);
  lv_obj_add_event_cb(btn_cfg, btn_config_cb, LV_EVENT_CLICKED, NULL);

  // Populate Data
  lv_label_set_text(lbl_ip, WiFi.localIP().toString().c_str());
  lv_label_set_text(lbl_ssid, WiFi.SSID().c_str());
  lv_label_set_text(lbl_mac, WiFi.macAddress().c_str());

  String rssi = String(WiFi.RSSI()) + " dBm";
  lv_label_set_text(lbl_rssi, rssi.c_str());
}
