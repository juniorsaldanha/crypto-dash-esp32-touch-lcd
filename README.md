# ESP32-S3 Touch LCD Crypto Monitor

A high-performance cryptocurrency monitor built for the **Waveshare ESP32-S3-Touch-LCD-2**. Features a smooth LVGL interface, real-time price updates from CoinGecko, and a web-based configuration portal.

## Features
-   **Live Prices**: Monitor up to 3 customizable cryptocurrencies in real-time (default: BTC, ETH, SOL).
-   **Multi-Currency Support**: Display prices in USD, EUR, BRL, or GBP with proper currency symbols.
-   **Smooth UI**: 30FPS+ interface powered by LVGL 8 and Arduino_GFX (Direct Mode).
-   **Touch Navigation**: Swipe through 3 main screens (Home Dashboard, Network Info, Settings).
-   **On-Device WiFi Config**: Configure WiFi credentials directly from the touchscreen with on-screen keyboard.
-   **Offline Persistence**: Cached crypto data survives reboots for instant display.
-   **NTP Time Sync**: Automatic time synchronization for accurate "Last Updated" timestamps.
-   **Web Admin Portal**: Built-in web server for advanced configuration.
-   **Robust Connectivity**: Auto-reconnect WiFi and failsafe API fetching.

## Hardware
-   **Board**: Waveshare ESP32-S3-Touch-LCD-2
-   **Display**: 2-inch IPS LCD (ST7789)
-   **Touch**: CST816S Capacitive Touch

## Project Structure
The project is organized into modular components:
```
src/
├── api/        # Crypto API fetching logic
├── core/       # Global configuration, types, and settings persistence
├── network/    # WiFi manager, NTP sync, and Web Server
├── ui/         # Display driver, Touch, UI Manager, and Screens
│   └── screens/  # Home, Network Info, Settings, WiFi Config
└── main.cpp    # Application entry point
```

## Setup & Building

### Prerequisites
-   VSCode
-   PlatformIO Extension

### Installation
1.  Clone this repository.
2.  Open in VSCode.
3.  PlatformIO will automatically download dependencies (LVGL, Arduino_GFX, ArduinoJson, etc.).

### Flashing
1.  Connect the board via USB-C (ensure you use the USB port, not just UART if separate).
2.  Run the upload command:
    ```bash
    pio run --target upload
    ```
    Or use the PlatformIO sidebar button.

### Debugging
To see logs (prices, status, memory usage):
```bash
pio device monitor
# or
just monitor
```

## Configuration

### WiFi Setup
1.  On first boot, if WiFi is not configured, the device will create a hotspot named **CryptoMonitor-AP**.
2.  Connect to it and configure your home WiFi credentials via the captive portal.
3.  Alternatively, use the **Network Info** screen → **Configure WiFi** button for on-device configuration.

### Customizing Cryptocurrencies
1.  Navigate to the **Settings** screen using the `<` or `>` buttons.
2.  Select your preferred currency (USD, EUR, BRL, GBP).
3.  Enter CoinGecko IDs and symbols for up to 3 cryptocurrencies.
4.  Tap **Save** to apply changes.

## Troubleshooting
-   **Display Blank?** Ensure you are using the correct `Arduino_GFX` version (pinned to 1.3.7 in `platformio.ini` to avoid incompatibility).
-   **Stuck on "Fetching"?** Check your internet connection. The device uses an insecure SSL client to maximize compatibility.
-   **Touch Not Working?** Verify the CST816S touch controller is properly connected (I2C pins).

## TODO / Future Features
- [ ] Monitor BTC wallet balance and transactions
- [ ] Display Bitcoin transaction fees from mempool.space (low, medium, high priority)
- [ ] Add Page Manager screen to enable/disable specific pages
- [ ] Integrate WebSocket API for real-time price updates (research free providers)
- [ ] Add price alerts and notifications
- [ ] Support for additional fiat currencies
- [ ] Historical price charts and trends
