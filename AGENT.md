# AGENT.md - Project Specification & Rules

## 1. Project Overview
**Name:** ESP32-S3 Crypto Monitor & Admin
**Hardware:** Waveshare ESP32-S3-Touch-LCD-2 (2-inch IPS, CST816 touch, OV2640 camera).
**Goal:** A high-performance cryptocurrency dashboard with multi-screen navigation, on-device WiFi configuration, and real-time price monitoring.

---

## 2. Hardware Abstraction Layer
* **MCU:** ESP32-S3 (Dual Core, 240MHz).
* **Memory:** Flash (16MB), PSRAM (8MB Octal/OPI - REQUIRED for Framebuffer).
* **Display:** ST7789 via **Arduino_GFX** (Direct Mode). Resolution 240x320.
* **Touch:** CST816S (I2C) via **bsp_cst816** local library.
* **Storage:** NVS (Preferences library) for config persistence and crypto data caching.

---

## 3. Architecture: Modular & Dual-Core

### **Project Structure**
The project follows a clean, modular architecture:
*   `src/core/`: Configuration (`config.h`), Types (`types.h`), and Settings Manager (`settings.cpp`).
*   `src/ui/`: Display driver (`display.cpp`), Touch driver (`touch.cpp`), UI Manager (`ui_manager.cpp`), and Screen implementations (`screens/`).
    *   `screens/screen_home.cpp`: Main dashboard with crypto prices.
    *   `screens/screen_network.cpp`: Network information display.
    *   `screens/screen_settings.cpp`: Currency and crypto configuration.
    *   `screens/screen_wifi_config.cpp`: On-device WiFi credential input.
*   `src/network/`: WiFi connection (`wifi_manager.cpp`), NTP sync (`ntp.cpp`), and Admin Portal (`web_server.cpp`).
*   `src/api/`: Crypto fetching logic (`crypto.cpp`).
*   `src/main.cpp`: Orchestrator (Setup & Task Initializers).

### **Threading Strategy**
*   **Core 0 (High Priority):** GUI Task. Handles LVGL rendering and Touch input. Driven by `lv_tick_inc` and `lv_timer_handler`.
*   **Core 1 (Medium Priority):** API Task. Fetches data via HTTP every 5 minutes, with semaphore-based immediate refresh on settings change.
*   **Main Loop:** Handles Web Server client requests.

### **Navigation System**
*   **3 Main Screens:** Home (Index 0), Network Info (Index 1), Settings (Index 2).
*   **Sub-Page:** WiFi Config (Index 3) - accessible from Network Info screen.
*   **Bottom Bar Controls:**
    *   `<` (Prev): Cycles backward through main screens.
    *   `O` (Home): Jumps directly to Home screen.
    *   `>` (Next): Cycles forward through main screens.

---

## 4. Software Stack (PlatformIO)
**Framework:** Arduino (Espressif32 Platform).
**Dependencies:**
1.  `moononournation/GFX Library for Arduino` (v1.3.7 pinned for compatibility).
2.  `lvgl/lvgl` (v8.4.0).
3.  `tzapu/WiFiManager` (Captive Portal for initial WiFi setup).
4.  `bblanchon/ArduinoJson` (JSON Parsing).
5.  `WebServer` (Built-in for admin portal).
6.  `Preferences` (Built-in for NVS storage).

---

## 5. Business Logic

### **Crypto API**
*   **Provider:** CoinGecko (Simple Price API - Free tier, no API key required).
*   **Endpoint:** `/simple/price`
*   **Strategy:** Fetch up to 3 configurable cryptocurrencies (default: BTC, ETH, SOL).
*   **Multi-Currency:** Supports USD, EUR, BRL, GBP with proper currency symbols.
*   **SSL:** `setInsecure()` used for stability.
*   **Caching:** All fetched data is cached to NVS for offline persistence.

### **Settings Management**
*   **Persistence:** All settings (currency, crypto IDs/symbols) stored in NVS.
*   **On-Device Configuration:** Full touchscreen interface for changing settings without web portal.
*   **Immediate Refresh:** Settings changes trigger immediate API refetch via semaphore.

### **WiFi Management**
*   **Initial Setup:** WiFiManager captive portal on first boot.
*   **On-Device Config:** Dedicated WiFi Config screen with on-screen keyboard for SSID/Password input.
*   **Reset Function:** Button to clear WiFi credentials and reboot into AP mode.

### **Admin Portal**
*   Accessible via IP after connection.
*   Provides web-based configuration alternative.
*   Displays current settings and allows remote management.

---

## 6. Performance Optimizations
*   **Compiler Flags:** `-O2` optimization enabled.
*   **Partial Screen Refresh:** LVGL configured for efficient rendering.
*   **Direct Mode:** Arduino_GFX uses direct framebuffer writes (no double buffering overhead).
*   **Cache-First Display:** Cached data displayed immediately on boot, then refreshed from API.

---

## 7. Future Enhancements (TODO)
- [ ] Monitor BTC wallet balance and transactions
- [ ] Display Bitcoin transaction fees from mempool.space (low, medium, high priority)
- [ ] Add Page Manager screen to enable/disable specific pages
- [ ] Integrate WebSocket API for real-time price updates
- [ ] Add price alerts and notifications
- [ ] Historical price charts
