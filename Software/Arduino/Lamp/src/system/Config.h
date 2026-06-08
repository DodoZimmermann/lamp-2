#pragma once

// ── GPIO Pins ─────────────────────────────────────────────────────────────────
#define PIN_LED_DATA     3
#define PIN_SCL          8
#define PIN_SDA          9
#define PIN_ALERT_TMP   11
#define PIN_ALERT_TOUCH 12
#define PIN_EN_5V_BOOST 13
#define PIN_EN_CHG      46

// ── I2C Addresses ─────────────────────────────────────────────────────────────
#define I2C_ADDR_TMP1075   0x48
#define I2C_ADDR_CAP1296   0x28

// ── LED ───────────────────────────────────────────────────────────────────────
#define LED_COUNT  60

// ── Temperature Thresholds ────────────────────────────────────────────────────
#define TEMP_ALERT_HIGH_C  60.0f
#define TEMP_ALERT_LOW_C    0.0f

// ── LED Effect Speeds ─────────────────────────────────────────────────────────
#define BREATHE_SPEED_MS  2000
#define RAINBOW_SPEED_MS  4000

// ── Scheduler Intervals (ms) ──────────────────────────────────────────────────
#define TASK_LED_MS        20
#define TASK_TOUCH_MS      50
#define TASK_TEMP_MS     1000
#define TASK_REMOTEXY_MS   10
