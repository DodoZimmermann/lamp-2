// Lamp v2 — ESP32-S3 | FastLED + TaskScheduler + RemoteXY
//
// Power architecture: LM73100 ideal diodes OR USB-C (15V buck → 5V)
//                     with battery boost (3.7V → 5V). Both paths feed 5V LED rail.
//                     EN_5V_BOOST held HIGH at boot for seamless switchover.

#include <TaskScheduler.h>

#include "src/system/Config.h"
#include "src/system/I2CManager.h"
#include "src/leds/LEDController.h"
#include "src/power/BatteryCharger.h"
#include "src/power/USBPDController.h"
#include "src/sensors/TempSensor.h"
#include "src/sensors/TouchSensor.h"

// ── RemoteXY ──────────────────────────────────────────────────────────────────
// TODO: Paste generated RemoteXY configuration block here.
// Recommended mode: REMOTEXY_MODE__WIFI_POINT  (lamp acts as AP, no router needed)
//
// #define REMOTEXY_MODE__WIFI_POINT
// #define REMOTEXY_WIFI_SSID     "Lamp"
// #define REMOTEXY_WIFI_PASSWORD "12345678"
// #define REMOTEXY_SERVER_PORT    6377
// #include <RemoteXY.h>
// struct { ... } RemoteXY;   // ← paste struct from RemoteXY editor
// ─────────────────────────────────────────────────────────────────────────────

// ── ISR flags (set in ISR, cleared in task callbacks) ─────────────────────────
volatile bool flagTempAlert  = false;
volatile bool flagTouchAlert = false;
volatile bool flagPdInt      = false;

void IRAM_ATTR isrTempAlert()  { flagTempAlert  = true; }
void IRAM_ATTR isrTouchAlert() { flagTouchAlert = true; }
void IRAM_ATTR isrPdInt()      { flagPdInt      = true; }

// ── Scheduler ─────────────────────────────────────────────────────────────────
Scheduler scheduler;

void cbLED();
void cbTouch();
void cbTemp();
void cbBattery();
void cbPower();
// void cbRemoteXY();  // uncomment when RemoteXY config is added

Task taskLED     (TASK_LED_MS,      TASK_FOREVER, &cbLED,     &scheduler, true);
Task taskTouch   (TASK_TOUCH_MS,    TASK_FOREVER, &cbTouch,   &scheduler, true);
Task taskTemp    (TASK_TEMP_MS,     TASK_FOREVER, &cbTemp,    &scheduler, true);
Task taskBattery (TASK_BATTERY_MS,  TASK_FOREVER, &cbBattery, &scheduler, true);
Task taskPower   (TASK_POWER_MS,    TASK_FOREVER, &cbPower,   &scheduler, true);
// Task taskRemoteXY(TASK_REMOTEXY_MS, TASK_FOREVER, &cbRemoteXY, &scheduler, false);

// ── Task Callbacks ────────────────────────────────────────────────────────────

void cbLED() {
    led.update();
}

void cbTouch() {
    if (flagTouchAlert) {
        flagTouchAlert = false;
        uint8_t pads = touchSensor.getTouchedPads();
        if (pads) Serial.printf("[TOUCH] Pads: 0b%06b\n", pads);
        // TODO: Map pad bits to lamp actions (e.g. pad 0 = toggle, pad 1/2 = brightness)
    }
}

void cbTemp() {
    if (flagTempAlert) {
        flagTempAlert = false;
        tempSensor.handleAlert();
        Serial.println("[TMP] ALERT triggered!");
    }
    float t = tempSensor.getTemperature();
    if (!isnan(t)) {
        Serial.printf("[TMP] %.2f °C\n", t);
        if (t >= TEMP_ALERT_HIGH_C) {
            // Over-temperature: dim LEDs to reduce heat
            led.setBrightness(20);
            Serial.println("[TMP] Over-temp — dimming LEDs");
        }
    }
}

void cbBattery() {
    charger.printStatus();
}

void cbPower() {
    // USB-C PD interrupt
    if (flagPdInt) {
        flagPdInt = false;
        usbPD.handleInterrupt();
    }

    bool usbcOk = digitalRead(PIN_PG_PD)  == HIGH;
    bool batOk  = digitalRead(PIN_PG_BAT) == HIGH;

    // Enable charging only when USB-C is present
    charger.enableCharging(usbcOk);

    if (!usbcOk && !batOk) {
        // Neither source is good — critical low-battery state
        Serial.println("[PWR] No power source — critical!");
        led.setEffect(Effect::BREATHING, 500);  // fast blink to warn
    }

    Serial.printf("[PWR] USB-C:%s  BAT:%s  CHG:%s\n",
                  usbcOk ? "OK" : "NO",
                  batOk  ? "OK" : "NO",
                  usbcOk ? "ON" : "OFF");
}

// void cbRemoteXY() {
//     RemoteXY_Handler();
//     // Read RemoteXY controls and apply:
//     // led.setBrightness(RemoteXY.slider_brightness);
//     // led.setColor(RemoteXY.rgb_r, RemoteXY.rgb_g, RemoteXY.rgb_b);
// }

// ── setup() ──────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("\n=== Lamp v2 boot ===");

    // 1. Enable battery boost immediately for seamless OR power (active HIGH)
    pinMode(PIN_EN_5V_BOOST, OUTPUT);
    digitalWrite(PIN_EN_5V_BOOST, HIGH);

    // 2. Output pins
    pinMode(PIN_EN_CHG, OUTPUT);
    digitalWrite(PIN_EN_CHG, LOW);  // Charger off until USB-C confirmed

    // 3. Input pins
    pinMode(PIN_PG_PD,       INPUT);
    pinMode(PIN_PG_BAT,      INPUT);
    pinMode(PIN_ALERT_TMP,   INPUT_PULLUP);
    pinMode(PIN_ALERT_TOUCH, INPUT_PULLUP);
    pinMode(PIN_PD_INT,      INPUT_PULLUP);
    pinMode(PIN_FAULT_PD,    INPUT_PULLUP);

    // 4. I2C
    i2c.begin(PIN_SDA, PIN_SCL);
    i2c.scan();

    // 5. USB-C PD controller — configure for 15V/3A
    usbPD.begin();

    // 6. Battery charger
    charger.begin();

    // 7. Temperature sensor
    tempSensor.begin();

    // 8. Capacitive touch
    touchSensor.begin();

    // 9. Interrupt service routines
    attachInterrupt(digitalPinToInterrupt(PIN_ALERT_TMP),   isrTempAlert,  FALLING);
    attachInterrupt(digitalPinToInterrupt(PIN_ALERT_TOUCH), isrTouchAlert, FALLING);
    attachInterrupt(digitalPinToInterrupt(PIN_PD_INT),      isrPdInt,      FALLING);

    // 10. LEDs — startup animation
    led.begin();
    led.setColor(255, 200, 120);  // warm white
    led.setEffect(Effect::BREATHING, 1500);

    // 11. RemoteXY — uncomment once config is pasted above
    // RemoteXY_Init();
    // taskRemoteXY.enable();

    Serial.println("=== Boot complete ===");
}

// ── loop() ───────────────────────────────────────────────────────────────────
void loop() {
    scheduler.execute();
}
