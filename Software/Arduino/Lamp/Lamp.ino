// Lamp v2 — ESP32-S3  |  FastLED + TaskScheduler + RemoteXY (BLE)

// ── RemoteXY ──────────────────────────────────────────────────────────────────
#define REMOTEXY_MODE__ESP32CORE_BLE
#include <BLEDevice.h>
#define REMOTEXY_BLUETOOTH_NAME "Lamp"

#include <RemoteXY.h>

#pragma pack(push, 1)
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] =  // 43 bytes V19
  { 255,5,0,0,0,36,0,19,0,0,0,0,31,1,106,200,1,1,3,0,
  6,23,67,57,57,2,26,4,30,145,48,16,128,2,26,3,17,12,72,20,
  132,2,26 };

struct {
    uint8_t rgb_01_r;
    uint8_t rgb_01_g;
    uint8_t rgb_01_b;
    int8_t  slider_01;         // 0–100
    uint8_t selectorSwitch_01; // 0=Off 1=Solid 2=Breathe 3=Rainbow
    uint8_t connect_flag;
} RemoteXY;
#pragma pack(pop)
// ─────────────────────────────────────────────────────────────────────────────

#include <TaskScheduler.h>
#include "src/system/Config.h"
#include "src/system/I2CManager.h"
#include "src/leds/LEDController.h"
#include "src/power/BatteryCharger.h"
#include "src/sensors/TempSensor.h"
#include "src/sensors/TouchSensor.h"

// ── ISR flags ────────────────────────────────────────────────────────────────
volatile bool flagTempAlert  = false;
volatile bool flagTouchAlert = false;

void IRAM_ATTR isrTempAlert()  { flagTempAlert  = true; }
void IRAM_ATTR isrTouchAlert() { flagTouchAlert = true; }

// ── Scheduler ────────────────────────────────────────────────────────────────
Scheduler scheduler;

void cbLED();
void cbTouch();
void cbTemp();
void cbRemoteXY();

Task taskLED     (TASK_LED_MS,      TASK_FOREVER, &cbLED,      &scheduler, true);
Task taskTouch   (TASK_TOUCH_MS,    TASK_FOREVER, &cbTouch,    &scheduler, false);
Task taskTemp    (TASK_TEMP_MS,     TASK_FOREVER, &cbTemp,     &scheduler, true);
Task taskRemoteXY(TASK_REMOTEXY_MS, TASK_FOREVER, &cbRemoteXY, &scheduler, true);

// ── Task Callbacks ────────────────────────────────────────────────────────────

void cbLED() {
    led.update();
}

void cbTouch() {
    if (!flagTouchAlert) return;
    flagTouchAlert = false;

    uint8_t pads = touchSensor.getTouchedPads();
    if (!pads) return;

    if (pads & 0x01)
        RemoteXY.selectorSwitch_01 = (RemoteXY.selectorSwitch_01 == 0) ? 1 : 0;
    if (pads & 0x02) {
        RemoteXY.slider_01 = constrain(RemoteXY.slider_01 + 10, 0, 100);
        led.setBrightness(RemoteXY.slider_01);
    }
    if (pads & 0x04) {
        RemoteXY.slider_01 = constrain(RemoteXY.slider_01 - 10, 0, 100);
        led.setBrightness(RemoteXY.slider_01);
    }
    if (pads & 0x08) {
        if (RemoteXY.selectorSwitch_01 == 0) RemoteXY.selectorSwitch_01 = 1;
        else RemoteXY.selectorSwitch_01 = (RemoteXY.selectorSwitch_01 % 3) + 1;
    }
    printf("[TOUCH] Pads: 0b%06b\n", pads);
}

void cbTemp() {
    if (flagTempAlert) {
        flagTempAlert = false;
        printf("[TMP] ALERT\n");
        led.setBrightness(20);
        RemoteXY.slider_01 = 20;
    }
    float t = tempSensor.getTemperature();
    if (!isnan(t)) printf("[TMP] %.2f C\n", t);
}

void cbRemoteXY() {
    RemoteXYEngine.handler();

    led.setBrightness(RemoteXY.slider_01);

    static uint8_t prev_r = 255, prev_g = 200, prev_b = 120;
    if (RemoteXY.rgb_01_r != prev_r || RemoteXY.rgb_01_g != prev_g || RemoteXY.rgb_01_b != prev_b) {
        prev_r = RemoteXY.rgb_01_r;
        prev_g = RemoteXY.rgb_01_g;
        prev_b = RemoteXY.rgb_01_b;
        led.setColor(prev_r, prev_g, prev_b);
    }

    static uint8_t prev_effect = 0xFF;
    if (RemoteXY.selectorSwitch_01 != prev_effect) {
        prev_effect = RemoteXY.selectorSwitch_01;
        switch (prev_effect) {
            case 0: led.setEffect(Effect::OFF);       break;
            case 1: led.setEffect(Effect::SOLID);     break;
            case 2: led.setEffect(Effect::BREATHING); break;
            case 3: led.setEffect(Effect::RAINBOW);   break;
        }
    }
}

// ── setup() ──────────────────────────────────────────────────────────────────
void setup() {
    printf("\n=== Lamp v2 boot ===\n");

    pinMode(PIN_EN_5V_BOOST, OUTPUT);
    digitalWrite(PIN_EN_5V_BOOST, HIGH);

    pinMode(PIN_ALERT_TMP,   INPUT_PULLUP);
    pinMode(PIN_ALERT_TOUCH, INPUT_PULLUP);

    i2c.begin(PIN_SDA, PIN_SCL);
    i2c.scan();

    charger.begin();
    charger.enable(true);

    tempSensor.begin();
    touchSensor.begin();

    attachInterrupt(digitalPinToInterrupt(PIN_ALERT_TMP),   isrTempAlert,  FALLING);
    attachInterrupt(digitalPinToInterrupt(PIN_ALERT_TOUCH), isrTouchAlert, FALLING);

    led.begin();
    led.setColor(255, 200, 120);
    led.setEffect(Effect::BREATHING);

    RemoteXY.slider_01         = 70;
    RemoteXY.rgb_01_r          = 255;
    RemoteXY.rgb_01_g          = 200;
    RemoteXY.rgb_01_b          = 120;
    RemoteXY.selectorSwitch_01 = 2;

    RemoteXY_Init();
    printf("[BLE] Advertising as 'Lamp'\n=== Boot complete ===\n");
}

// ── loop() ───────────────────────────────────────────────────────────────────
void loop() {
    scheduler.execute();
}
