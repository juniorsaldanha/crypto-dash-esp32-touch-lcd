#pragma once

// Display Pins (ST7789)
#define PIN_LCD_SCLK 39
#define PIN_LCD_MOSI 38
#define PIN_LCD_MISO 40
#define PIN_LCD_DC 42
#define PIN_LCD_RST -1
#define PIN_LCD_CS 45
#define PIN_LCD_BL 1

// Touch Pins (CST816S)
#define PIN_TP_SDA 48
#define PIN_TP_SCL 47

// Display Resolution & Orientation
#define LCD_ROTATION 0
#define LCD_H_RES 240
#define LCD_V_RES 320

// API Configuration
#define API_TIMEOUT_MS 10000
