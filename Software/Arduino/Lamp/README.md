# Lamp v2 — ESP32-S3 Firmware

Custom firmware for a self-built LED lamp with USB-C Power Delivery, LiPo battery, capacitive touch, and Bluetooth control.

---

## Hardware

| Component | Part | Interface |
|---|---|---|
| MCU | ESP32-S3 | — |
| LEDs | 60× WS2812B | GPIO (RMT) |
| USB-C PD controller | CYPD3177 | I2C |
| Battery charger | BQ25303JRTER | I2C + GPIO |
| Ideal diode OR | 2× LM73100 | — |
| Temperature sensor | TMP1075 | I2C |
| Capacitive touch | CAP1296 (6-channel) | I2C |
| Buck converter | 15V → 5V (USB-C path) | — |
| Boost converter | 3.7V → 5V (battery path) | GPIO enable |

### Power Architecture

The lamp runs off two power paths that are OR'd together by a pair of LM73100 ideal diodes — switchover is seamless with no firmware intervention:

```
USB-C (15V) ──► Buck ──► 5V ──► [LM73100] ──► 5V LED rail
                                                    ▲
Battery (3.7V) ──► Boost ──► 5V ──► [LM73100] ──┘
```

- When USB-C is present: LEDs are powered via the buck, battery charges via BQ25303J
- When USB-C is absent: boost converter supplies 5V from battery
- `EN_5V_BOOST` is pulled HIGH at boot so the battery path is always ready

---

## Pinout

| Signal | GPIO |
|---|---|
| LED Data (WS2812B) | IO3 |
| I2C SCL | IO8 |
| I2C SDA | IO9 |
| TMP1075 ALERT | IO11 |
| CAP1296 ALERT | IO12 |
| Boost enable (active HIGH) | IO13 |
| CYPD3177 PD_INT | IO14 |
| CYPD3177 FAULT | IO17 |
| BQ25303J charge enable (active HIGH) | IO46 |
| LM73100 PG — USB-C path | IO47 |
| LM73100 PG — battery path | IO48 |

---

## Software Architecture

```
Lamp/
  Lamp.ino                   ← setup(), loop(), scheduler tasks
  src/
    leds/
      LEDController.h/.cpp   ← FastLED, effects (Solid/Breathing/Rainbow/Off)
    power/
      BatteryCharger.h/.cpp  ← BQ25303J status via I2C, GPIO charge enable
      USBPDController.h/.cpp ← CYPD3177 HPI, requests 15V/3A PDO at boot
    sensors/
      TempSensor.h/.cpp      ← TMP1075, alert thresholds, temperature read
      TouchSensor.h/.cpp     ← CAP1296, interrupt-driven touch detection
    system/
      I2CManager.h/.cpp      ← shared Wire bus, 8-bit and 16-bit register access
      Config.h               ← all pin definitions, I2C addresses, constants
```

### Scheduler Tasks (TaskScheduler library)

| Task | Interval | Purpose |
|---|---|---|
| `taskLED` | 20 ms | Advance LED animation, call `FastLED.show()` |
| `taskTouch` | 50 ms | Process CAP1296 alert flag |
| `taskTemp` | 5 000 ms | Read TMP1075, log, dim on over-temp |
| `taskBattery` | 10 000 ms | Log BQ25303J charge state and fault register |
| `taskPower` | 1 000 ms | Manage charge enable, detect no-power condition |
| `taskRemoteXY` | 10 ms | `RemoteXYEngine.handler()`, apply phone controls |

### LED Effects

| Effect | Description |
|---|---|
| `SOLID` | All LEDs at set color and brightness |
| `BREATHING` | Sinusoidal brightness oscillation (default 2 s period) |
| `RAINBOW` | Hue sweep across all LEDs (default 3 s period) |
| `OFF` | All LEDs off |

---

## Remote Control (RemoteXY via BLE)

Connect with the [RemoteXY app](https://remotexy.com/en/download/) — pair to Bluetooth device **"Lamp"**.

| Control | Action |
|---|---|
| Power toggle | On / Off |
| Brightness slider | 0 – 100 % |
| RGB color picker | Sets LED color (switches to Solid mode) |
| Effect selector | Solid / Breathe / Rainbow |

### Touch Controls (physical pads on lamp)

| Pad | Action |
|---|---|
| 0 | Toggle power |
| 1 | Brightness +10 % |
| 2 | Brightness −10 % |
| 3 | Cycle effect (Solid → Breathe → Rainbow → …) |

---

## Required Libraries

Install all via **Arduino IDE → Tools → Manage Libraries**:

| Library | Author |
|---|---|
| FastLED | Daniel Garcia |
| TaskScheduler | Anatoli Arkhipenko |
| RemoteXY | Rem (v4.1.8 or later) |

Board package: **esp32** by Espressif (Board Manager) — select **ESP32S3 Dev Module**.

---

## Building & Flashing

1. Open `Lamp/Lamp.ino` in Arduino IDE
2. Select board: **ESP32S3 Dev Module**
3. Set **USB CDC On Boot: Enabled** (for Serial output)
4. Install the three libraries above
5. Compile and upload

---

## Known TODOs / Caveats

- **CYPD3177 PDO index**: `PDO_INDEX_15V = 0x03` assumes the connected charger advertises PDOs in order 5V / 9V / 12V / 15V. If 15V is not negotiated, verify the actual PDO order with a USB-PD analyzer and adjust the constant in `USBPDController.h`.
- **BQ25303J I2C address**: set to `0x6B` (default). Verify against the `ADDR` pin configuration on your PCB — may be `0x2D`.
- **RemoteXY input variables are one-way**: brightness/effect changes made via touch pads are applied to the LEDs immediately but are not reflected back on the phone UI (RemoteXY limitation).
