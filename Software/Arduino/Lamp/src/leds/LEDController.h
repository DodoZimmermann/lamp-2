#pragma once
#include <Arduino.h>
#include <FastLED.h>
#include "../system/Config.h"

enum class Effect { OFF, SOLID, BREATHING, RAINBOW };

class LEDController {
public:
    void begin();
    void setBrightness(uint8_t pct);          // 0–100 %
    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void setEffect(Effect e, uint16_t speedMs = 2000);
    void update();                             // called every TASK_LED_MS

    uint8_t  brightness() const { return _brightness; }
    CRGB     color()      const { return _color; }
    Effect   effect()     const { return _effect; }

private:
    CRGB     _leds[LED_COUNT];
    Effect   _effect     = Effect::OFF;
    uint8_t  _brightness = 50;   // %
    CRGB     _color      = CRGB::White;
    uint16_t _speedMs    = 2000;

    void _updateBreathing();
    void _updateRainbow();
};

extern LEDController led;
