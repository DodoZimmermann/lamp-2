#pragma once

// ── GPIO Pins ─────────────────────────────────────────────────────────────────
#define PIN_LED_DATA     3
#define PIN_SCL          8
#define PIN_SDA          9
#define PIN_ALERT_TMP   11
#define PIN_ALERT_TOUCH 12
#define PIN_EN_5V_BOOST 13   // Active HIGH — enables battery 3.7V → 5V boost
#define PIN_PD_INT      14
#define PIN_FAULT_PD    17
#define PIN_EN_CHG      46   // Active HIGH — enables BQ25303J charging
#define PIN_PG_PD       47   // HIGH = USB-C ideal diode output is good (LM73100)
#define PIN_PG_BAT      48   // HIGH = battery boost ideal diode output is good (LM73100)

// ── I2C Addresses ─────────────────────────────────────────────────────────────
// TODO: Confirm BQ25303J address based on ADDR pin configuration (may be 0x6B)
#define I2C_ADDR_BQ25303J  0x6B
#define I2C_ADDR_CYPD3177  0x08
#define I2C_ADDR_TMP1075   0x48  // A0=A1=A2=GND
#define I2C_ADDR_CAP1296   0x28

// ── LED ───────────────────────────────────────────────────────────────────────
#define LED_COUNT  60

// ── USB-PD Target ─────────────────────────────────────────────────────────────
#define PD_TARGET_VOLTAGE_MV  15000
#define PD_TARGET_CURRENT_MA   3000

// ── Temperature Thresholds ────────────────────────────────────────────────────
#define TEMP_ALERT_HIGH_C  60.0f
#define TEMP_ALERT_LOW_C    0.0f

// ── Scheduler Intervals (ms) ──────────────────────────────────────────────────
#define TASK_LED_MS        20
#define TASK_TOUCH_MS      50
#define TASK_TEMP_MS     5000
#define TASK_BATTERY_MS 10000
#define TASK_POWER_MS    1000
#define TASK_REMOTEXY_MS   10
