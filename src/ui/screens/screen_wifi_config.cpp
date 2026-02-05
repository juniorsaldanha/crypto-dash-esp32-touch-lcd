#include "../../network/wifi_manager.h"
#include "../ui_manager.h"
#include "screens.h"
#include <WiFi.h>

static lv_obj_t *ta_ssid;
static lv_obj_t *ta_pass;
static lv_obj_t *kb;

static void ta_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *ta = lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED || code == LV_EVENT_FOCUSED) {
    if (kb != NULL) {
      lv_keyboard_set_textarea(kb, ta);
      lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
  } else if (code == LV_EVENT_READY) {
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_state(ta, LV_STATE_FOCUSED);
    lv_indev_reset(NULL, ta);
  }
}

static void btn_connect_cb(lv_event_t *e) {
  const char *ssid = lv_textarea_get_text(ta_ssid);
  const char *pass = lv_textarea_get_text(ta_pass);
  wifi_connect(ssid, pass);
  lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
  // Optionally buffer a message or go back? staying here allows retrying if
  // failed (though we don't show failure yet)
}

static void btn_reset_cb(lv_event_t *e) { wifi_reset_settings(); }

static void btn_back_cb(lv_event_t *e) {
  load_screen(1); // Back to Network Info (Index 1)
}

void init_screen_wifi_config(lv_obj_t *parent) {
  lv_obj_t *cont = lv_obj_create(parent);
  lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
  lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(cont, 10, 0);
  lv_obj_set_style_bg_color(cont, lv_color_hex(0x0d1117), 0);
  lv_obj_set_style_border_width(cont, 0, 0);

  // Header Row
  lv_obj_t *header = lv_obj_create(cont);
  lv_obj_set_width(header, lv_pct(100));
  lv_obj_set_height(header, LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_bg_opa(header, 0, 0);
  lv_obj_set_style_border_width(header, 0, 0);
  lv_obj_set_style_pad_all(header, 0, 0);
  lv_obj_set_style_pad_gap(header, 10, 0);

  // Back Button
  lv_obj_t *btn_back = lv_btn_create(header);
  lv_obj_set_size(btn_back, 40, 40);
  lv_obj_set_style_bg_color(btn_back, lv_color_hex(0x30363d), 0);
  lv_obj_add_event_cb(btn_back, btn_back_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *l_back = lv_label_create(btn_back);
  lv_label_set_text(l_back, LV_SYMBOL_LEFT);
  lv_obj_center(l_back);

  lv_obj_t *title = lv_label_create(header);
  lv_label_set_text(title, "WiFi Settings");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(title, lv_color_white(), 0);

  // SSID Input
  ta_ssid = lv_textarea_create(cont);
  lv_textarea_set_placeholder_text(ta_ssid, "SSID");
  lv_textarea_set_one_line(ta_ssid, true);
  lv_obj_set_width(ta_ssid, lv_pct(100));
  lv_obj_add_event_cb(ta_ssid, ta_event_cb, LV_EVENT_ALL, NULL);

  // PASS Input
  ta_pass = lv_textarea_create(cont);
  lv_textarea_set_placeholder_text(ta_pass, "Password");
  lv_textarea_set_password_mode(ta_pass, true);
  lv_textarea_set_one_line(ta_pass, true);
  lv_obj_set_width(ta_pass, lv_pct(100));
  lv_obj_add_event_cb(ta_pass, ta_event_cb, LV_EVENT_ALL, NULL);

  // Buttons Row
  lv_obj_t *btn_row = lv_obj_create(cont);
  lv_obj_set_size(btn_row, lv_pct(100), LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(btn_row, LV_FLEX_FLOW_ROW);
  lv_obj_set_style_bg_opa(btn_row, 0, 0);
  lv_obj_set_style_border_width(btn_row, 0, 0);
  lv_obj_set_style_pad_all(btn_row, 0, 0);
  lv_obj_set_flex_align(btn_row, LV_FLEX_ALIGN_SPACE_BETWEEN,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  // Connect Button
  lv_obj_t *btn_conn = lv_btn_create(btn_row);
  lv_obj_set_width(btn_conn, lv_pct(48));
  lv_obj_set_style_bg_color(btn_conn, lv_color_hex(0x238636), 0);
  lv_obj_t *l_conn = lv_label_create(btn_conn);
  lv_label_set_text(l_conn, "Save");
  lv_obj_center(l_conn);
  lv_obj_add_event_cb(btn_conn, btn_connect_cb, LV_EVENT_CLICKED, NULL);

  // Reset Button
  lv_obj_t *btn_rst = lv_btn_create(btn_row);
  lv_obj_set_width(btn_rst, lv_pct(48));
  lv_obj_set_style_bg_color(btn_rst, lv_color_hex(0xda3633), 0);
  lv_obj_t *l_rst = lv_label_create(btn_rst);
  lv_label_set_text(l_rst, "Reset WiFi");
  lv_obj_center(l_rst);
  lv_obj_add_event_cb(btn_rst, btn_reset_cb, LV_EVENT_CLICKED, NULL);

  // Keyboard (Create last to float on top or be at bottom)
  kb = lv_keyboard_create(lv_scr_act()); // Create on active screen to float
  lv_obj_set_size(kb, lv_pct(100), lv_pct(50));
  lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

  // Pre-fill SSID if connected
  if (WiFi.status() == WL_CONNECTED) {
    lv_textarea_set_text(ta_ssid, WiFi.SSID().c_str());
  }
}
