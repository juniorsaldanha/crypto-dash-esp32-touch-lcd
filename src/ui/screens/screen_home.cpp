#include "../../core/settings.h"
#include "../../network/ntp.h"
#include "screens.h"
#include <stdio.h>

static lv_obj_t *price_labels[3];
static lv_obj_t *symbol_labels[3]; // Added missing global
static lv_obj_t *change_labels[3];
static lv_obj_t *time_label;
static unsigned long last_updates[3] = {0, 0, 0};

void init_screen_home(lv_obj_t *parent) {
  lv_obj_t *cont = lv_obj_create(parent);
  lv_obj_set_size(cont, lv_pct(100),
                  lv_pct(100)); // Fill parent (which is the tab content area)
  lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(cont, 5, 0); // Reduced padding
  lv_obj_set_style_bg_color(cont, lv_color_hex(0x0d1117), 0);
  lv_obj_set_style_border_width(cont, 0, 0);

  // Time Label at top
  time_label = lv_label_create(cont);
  lv_label_set_text(time_label, "Loading...");
  lv_obj_set_style_text_color(time_label, lv_color_hex(0x8b949e), 0);
  lv_obj_set_style_text_align(time_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_width(time_label, lv_pct(100));

  // Crypto Cards
  // We fetch symbols from Settings? Ideally yes, but for now loop 3 slots
  // The update function receives specific data for slots

  for (int i = 0; i < 3; i++) {
    lv_obj_t *card = lv_obj_create(cont);
    lv_obj_set_width(card, lv_pct(100));
    lv_obj_set_flex_grow(card, 1);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x161b22), 0);
    lv_obj_set_style_border_color(card, lv_color_hex(0x30363d), 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_pad_all(card, 8, 0);
    lv_obj_set_flex_flow(card, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(card, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // We don't set symbol text here anymore, we wait for update?
    // Or we create a label that gets updated.
    // Actually the current data flow sends symbol name "BTC", "ETH".
    // But the card needs a label object to update.

    // Left Column (Symbol + Name maybe?)
    lv_obj_t *left_col = lv_obj_create(card);
    lv_obj_set_size(left_col, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(left_col, 0, 0);
    lv_obj_set_style_border_width(left_col, 0, 0);
    lv_obj_set_flex_flow(left_col, LV_FLEX_FLOW_COLUMN);

    // Placeholder symbol label, we need to store it to update it if we want
    // dynamic symbols? Current architecture relies on hardcoded order in
    // main.cpp/crypto.cpp queue sending. crypto.cpp sends BTC first, then ETH,
    // then SOL. update_home_screen checks symbol string to decide index. So we
    // can just create labels and let update_home_screen set the text if we
    // want. Or we assume the user configured symbols match the slots.

    // Let's create a label for the Symbol Name that we can try to update or
    // just leave static for now? User requested settings page to replace
    // cryptos. So symbols should be dynamic. I'll create a label store array
    // for symbols too.

    lv_obj_t *sym = lv_label_create(left_col);
    lv_label_set_text(sym, "---"); // Will be updated
    lv_obj_set_style_text_font(sym, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(sym, lv_color_white(), 0);
    // Ensure we can access this to update...
    // For simplicity let's rely on the update function locating it or just
    // create a global array for it. Since I only have price_labels global, I
    // might need symbol_labels global. I will add symbol_labels global array.
    lv_obj_set_user_data(sym, (void *)(intptr_t)i);
    symbol_labels[i] = sym; // Assign to global array

    // Right Column (Price + Change)
    lv_obj_t *right_col = lv_obj_create(card);
    lv_obj_set_size(right_col, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(right_col, 0, 0);
    lv_obj_set_style_border_width(right_col, 0, 0);
    lv_obj_set_flex_flow(right_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(right_col, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_END,
                          LV_FLEX_ALIGN_END);

    price_labels[i] = lv_label_create(right_col);
    lv_label_set_text(price_labels[i], "$---");
    lv_obj_set_style_text_font(price_labels[i], &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(price_labels[i], lv_color_hex(0x00ff88), 0);

    change_labels[i] = lv_label_create(right_col);
    lv_label_set_text(change_labels[i], "--%");
    lv_obj_set_style_text_font(change_labels[i], &lv_font_montserrat_12,
                               0); // Increased size
    lv_obj_set_style_text_align(change_labels[i], LV_TEXT_ALIGN_RIGHT, 0);
    lv_label_set_recolor(change_labels[i], true); // Enable inline coloring

    // Attempt to load from cache
    CryptoData cached;
    if (load_crypto_cache(i, &cached)) {
      // We have data! Process it immediately
      symbol_labels[i] = sym; // Ensure global is set first
      update_home_screen(cached);
    }
  }
}

// Global array for symbol labels (needs to be defined at top if used, adding it
// now)

// MODIFIED LOOP to assign symbol_labels
// ... inside loop
// symbol_labels[i] = sym;

void update_home_screen(const CryptoData &data) {
  // Current logic maps symbol string to Index.
  // Ideally we should just fill slots 0, 1, 2 based on arrival order if
  // consistent, OR we match against Settings (which we can read here).

  // Simpler: The queue receiver in main.cpp doesn't know the index.
  // But crypto.cpp sends them in order? No, async.
  // crypto.cpp uses settings to determine what to fetch.

  // Let's look up the index based on the symbol passed in data vs settings.
  AppSettings settings = get_settings();
  int idx = -1;
  // Simple check
  if (strcmp(data.symbol, settings.crypto_symbols[0]) == 0)
    idx = 0;
  else if (strcmp(data.symbol, settings.crypto_symbols[1]) == 0)
    idx = 1;
  else if (strcmp(data.symbol, settings.crypto_symbols[2]) == 0)
    idx = 2;

  if (idx == -1)
    return; // Unknown symbol

  last_updates[idx] = data.timestamp;

  // Update Symbol Label (in case it changed)
  if (symbol_labels[idx])
    lv_label_set_text(symbol_labels[idx], data.symbol);

  char buf[32];
  const char *curr_symbol = "$";

  float price = 0.0;
  float change = 0.0;

  // Determine currency symbol and values
  if (strcmp(settings.currency, "usd") == 0 ||
      strcmp(settings.currency, "USD") == 0) {
    curr_symbol = "$";
    price = data.price_usd;
    change = data.change_usd;
  } else if (strcmp(settings.currency, "eur") == 0 ||
             strcmp(settings.currency, "EUR") == 0) {
    curr_symbol = "€";
    price = data.price_eur;
    change = data.change_eur;
  } else if (strcmp(settings.currency, "brl") == 0 ||
             strcmp(settings.currency, "BRL") == 0) {
    curr_symbol = "R$";
    price = data.price_brl;
    change = data.change_brl;
  } else if (strcmp(settings.currency, "gbp") == 0 ||
             strcmp(settings.currency, "GBP") == 0) {
    curr_symbol = "£";
    price = data.price_gbp;
    change = data.change_gbp;
  }

  // Formatter based on price magnitude
  if (price < 1.0)
    snprintf(buf, sizeof(buf), "%s%.4f", curr_symbol, price);
  else
    snprintf(buf, sizeof(buf), "%s%.2f", curr_symbol,
             price); // At least 2 decimals

  lv_label_set_text(price_labels[idx], buf);

  // 24h Change Only
  char buf_24h[64];
  const char *color_24h = (change >= 0) ? "#00ff88" : "#ff4444";
  snprintf(buf_24h, sizeof(buf_24h), "%s %.1f%%# #666666 - 24h#", color_24h,
           change);

  lv_label_set_text(change_labels[idx], buf_24h);
}

void check_home_stale_data() {
  unsigned long now = millis();
  for (int i = 0; i < 3; i++) {
    if (now - last_updates[i] > 300000UL && last_updates[i] != 0) {
      lv_label_set_text(change_labels[i], "STALE");
      lv_obj_set_style_text_color(change_labels[i], lv_color_hex(0x666666), 0);
    }
  }
}

void update_time_label() {
  if (time_label) {
    String ts = get_current_date() + " " + get_current_time();
    lv_label_set_text(time_label, ts.c_str());
  }
}
