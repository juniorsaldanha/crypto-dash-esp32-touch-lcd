#include "ui_manager.h"
#include "../core/config.h"
#include "screens/screens.h"

static lv_obj_t *main_content;
static lv_obj_t *bottom_bar;
static int current_screen_index = 0; // 0=Home, 1=Network, 2=Settings

void load_screen(int index) {
  lv_obj_clean(main_content);
  current_screen_index = index;

  if (index == 0)
    init_screen_home(main_content);
  else if (index == 1)
    init_screen_network(main_content);
  else if (index == 2)
    init_screen_settings(main_content);
  else if (index == 3)
    init_screen_wifi_config(main_content);
}

static void btn_handler(lv_event_t *e) {
  int btn_id = (int)(intptr_t)lv_event_get_user_data(e);

  if (btn_id == 1) { // Home Button -> Index 0
    load_screen(0);
  } else if (btn_id == 0) { // Prev (<)
    int next = current_screen_index - 1;
    if (next < 0)
      next = 2; // Wrap 0 -> 2
    // If we are deep in sub-pages (3), decide behavior?
    // Let's assume hitting prev from 3 goes to 2?
    // Or just strictly cycle 0-1-2.
    if (current_screen_index > 2)
      next = 2;
    load_screen(next);
  } else if (btn_id == 2) { // Next (>)
    int next = current_screen_index + 1;
    if (next > 2)
      next = 0; // Wrap 2 -> 0
    if (current_screen_index > 2)
      next = 0;
    load_screen(next);
  }
}

void init_ui() {
  // ... (Container setup skipped, generic)

  // 1. Main Container (Top part)
  lv_obj_t *scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
  lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(scr, 0, 0);
  lv_obj_set_style_pad_gap(scr, 0, 0);

  main_content = lv_obj_create(scr);
  lv_obj_set_width(main_content, lv_pct(100));
  lv_obj_set_flex_grow(main_content, 1);
  lv_obj_set_style_border_width(main_content, 0, 0);
  lv_obj_set_style_radius(main_content, 0, 0);
  lv_obj_set_style_pad_all(main_content, 0, 0);
  lv_obj_set_style_bg_color(main_content, lv_color_hex(0x0d1117), 0);

  // 2. Bottom Bar (Fixed height)
  bottom_bar = lv_obj_create(scr);
  lv_obj_set_width(bottom_bar, lv_pct(100));
  lv_obj_set_height(bottom_bar, 50);
  lv_obj_set_style_bg_color(bottom_bar, lv_color_hex(0x161b22), 0);
  lv_obj_set_style_border_side(bottom_bar, LV_BORDER_SIDE_TOP, 0);
  lv_obj_set_style_border_color(bottom_bar, lv_color_hex(0x30363d), 0);
  lv_obj_set_style_border_width(bottom_bar, 1, 0);
  lv_obj_set_flex_flow(bottom_bar, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(bottom_bar, LV_FLEX_ALIGN_SPACE_AROUND,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_all(bottom_bar, 0, 0);

  auto create_nav_btn = [&](const char *txt, int id) {
    lv_obj_t *btn = lv_btn_create(bottom_bar);
    lv_obj_set_size(btn, 60, 40);
    lv_obj_set_style_bg_opa(btn, 0, 0);
    lv_obj_add_event_cb(btn, btn_handler, LV_EVENT_CLICKED,
                        (void *)(intptr_t)id);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, txt);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
    lv_obj_center(label);
  };

  create_nav_btn("<", 0);
  create_nav_btn("O", 1);
  create_nav_btn(">", 2);

  load_screen(0); // Start at Home
}

void update_ui_tasks() {
  if (current_screen_index == 0) { // Home
    update_time_label();
    check_home_stale_data();
  }
}

void update_data_on_ui(const CryptoData &data) {
  if (current_screen_index == 0) { // Home
    update_home_screen(data);
  }
}
