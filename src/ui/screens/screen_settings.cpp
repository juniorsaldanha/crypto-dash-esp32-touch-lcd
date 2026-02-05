#include "../../api/crypto.h"
#include "../../core/settings.h"
#include "../ui_manager.h"
#include "screens.h"

static lv_obj_t *dd_currency;
static lv_obj_t *ta_crypto_id[3];
static lv_obj_t *ta_crypto_sym[3];
static lv_obj_t *kb;

static void ta_event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *ta = lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED || code == LV_EVENT_FOCUSED) {
    if (kb != NULL) {
      lv_keyboard_set_textarea(kb, ta);
      lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
  }
  if (code == LV_EVENT_DEFOCUSED) {
    if (kb != NULL) {
      lv_keyboard_set_textarea(kb, NULL);
      lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
  }
}

static void save_event_handler(lv_event_t *e) {
  // Read values and save
  AppSettings settings = get_settings();

  // Currency
  uint16_t opt = lv_dropdown_get_selected(dd_currency);
  char buf[16];
  lv_dropdown_get_selected_str(dd_currency, buf, sizeof(buf));
  strlcpy(settings.currency, buf, sizeof(settings.currency));

  // Determine if we need to refetch (simplistic: always refetch if saving
  // settings) Or check against old values. For now, let's assume we refetch to
  // be safe.
  bool symbols_changed = false;

  // Cryptos
  for (int i = 0; i < 3; i++) {
    const char *txt_id = lv_textarea_get_text(ta_crypto_id[i]);
    const char *txt_sym = lv_textarea_get_text(ta_crypto_sym[i]);

    // Check for changes before overwriting?
    // Simplified: Just overwrite and trigger update.
    if (strlen(txt_id) > 0) {
      if (strcmp(settings.crypto_ids[i], txt_id) != 0)
        symbols_changed = true;
      strlcpy(settings.crypto_ids[i], txt_id, sizeof(settings.crypto_ids[i]));
    }
    if (strlen(txt_sym) > 0) {
      char sym[8];
      strlcpy(sym, txt_sym, sizeof(sym));
      strupr(sym);
      if (strcmp(settings.crypto_symbols[i], sym) != 0)
        symbols_changed = true;
      strlcpy(settings.crypto_symbols[i], sym,
              sizeof(settings.crypto_symbols[i]));
    }
  }

  save_settings(settings);

  lv_obj_t *btn = lv_event_get_target(e);
  lv_obj_t *label = lv_obj_get_child(btn, 0);

  // Logic from previous version:
  // If symbols changed, we MUST fetch.
  // If only currency changed, we can try to reload from cache and just update
  // UI

  if (symbols_changed) {
    lv_label_set_text(label, "Updating...");
    xSemaphoreGive(fetch_sem); // Trigger fetch
    load_screen(0);            // Navigate to Home
  } else {
    // Maybe currency changed.
    // Update Home Screen with new Currency symbol using cached data?
    // For now, let's just reload.
    lv_label_set_text(label, "Saved!");

    // Reload cache to refresh currency symbol on Home
    for (int i = 0; i < 3; i++) {
      CryptoData cached;
      if (load_crypto_cache(i, &cached)) {
        update_home_screen(cached);
      }
    }
    load_screen(0); // Navigate to Home
  }
}

void init_screen_settings(lv_obj_t *parent) {
  lv_obj_t *cont = lv_obj_create(parent);
  lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
  lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(cont, 10, 0);
  lv_obj_set_style_bg_color(cont, lv_color_hex(0x0d1117), 0);
  lv_obj_set_style_border_width(cont, 0, 0);

  lv_obj_t *title = lv_label_create(cont);
  lv_label_set_text(title, "Settings");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(title, lv_color_white(), 0);

  AppSettings settings = get_settings();

  // Currency
  lv_obj_t *lbl_curr = lv_label_create(cont);
  lv_label_set_text(lbl_curr, "Currency");
  lv_obj_set_style_text_color(lbl_curr, lv_color_hex(0x8b949e), 0);

  dd_currency = lv_dropdown_create(cont);
  lv_dropdown_set_options(dd_currency, "USD\nEUR\nBRL\nGBP");
  lv_obj_set_width(dd_currency, lv_pct(100));

  // Set current selection
  if (strstr(settings.currency, "EU") || strstr(settings.currency, "eu"))
    lv_dropdown_set_selected(dd_currency, 1);
  else if (strstr(settings.currency, "BR") || strstr(settings.currency, "br"))
    lv_dropdown_set_selected(dd_currency, 2);
  else if (strstr(settings.currency, "GB") || strstr(settings.currency, "gb"))
    lv_dropdown_set_selected(dd_currency, 3);
  else
    lv_dropdown_set_selected(dd_currency, 0);

  // Cryptos Buttons
  for (int i = 0; i < 3; i++) {
    lv_obj_t *lbl = lv_label_create(cont);
    char buf[32];
    snprintf(buf, sizeof(buf), "Coin %d (ID / Ticker)", i + 1);
    lv_label_set_text(lbl, buf);
    lv_obj_set_style_text_color(lbl, lv_color_hex(0x8b949e), 0);

    lv_obj_t *row = lv_obj_create(cont);
    lv_obj_set_size(row, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_bg_opa(row, 0, 0);
    lv_obj_set_style_border_width(row, 0, 0);

    // ID Input
    ta_crypto_id[i] = lv_textarea_create(row);
    lv_textarea_set_one_line(ta_crypto_id[i], true);
    lv_textarea_set_placeholder_text(ta_crypto_id[i], "ID (bitcoin)");
    lv_textarea_set_text(ta_crypto_id[i], settings.crypto_ids[i]);
    lv_obj_set_width(ta_crypto_id[i], lv_pct(65));
    lv_obj_add_event_cb(ta_crypto_id[i], ta_event_handler, LV_EVENT_ALL, NULL);

    // Symbol Input
    ta_crypto_sym[i] = lv_textarea_create(row);
    lv_textarea_set_one_line(ta_crypto_sym[i], true);
    lv_textarea_set_placeholder_text(ta_crypto_sym[i], "Sym");
    lv_textarea_set_text(ta_crypto_sym[i], settings.crypto_symbols[i]);
    lv_obj_set_width(ta_crypto_sym[i], lv_pct(30));
    lv_obj_set_style_pad_left(ta_crypto_sym[i], 10, 0); // Spacing
    lv_obj_add_event_cb(ta_crypto_sym[i], ta_event_handler, LV_EVENT_ALL, NULL);
  }

  // Save Button
  lv_obj_t *btn_save = lv_btn_create(cont);
  lv_obj_set_width(btn_save, lv_pct(100));
  lv_obj_add_event_cb(btn_save, save_event_handler, LV_EVENT_CLICKED, NULL);

  lv_obj_t *lbl_btn = lv_label_create(btn_save);
  lv_label_set_text(lbl_btn, "Save");
  lv_obj_center(lbl_btn);

  // Keyboard (Hidden by default)
  kb = lv_keyboard_create(lv_scr_act()); // Create on active screen (top layer)
  lv_obj_set_size(kb, LV_HOR_RES, LV_VER_RES / 2);
  lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
}
