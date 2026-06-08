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
    void setEffect(Effect e);
    void update();                             // called every TASK_LED_MS

    void     setIndicator(uint8_t idx, CRGB color, bool active);

    uint8_t  brightness() const { return _brightness; }
    CRGB     color()      const { return _color; }
    Effect   effect()     const { return _effect; }

private:
    CRGB     _leds[LED_COUNT];
    Effect   _effect          = Effect::OFF;
    uint8_t  _brightness      = 50;   // %
    CRGB     _color           = CRGB::White;
    bool     _indicatorActive = false;
    uint8_t  _indicatorIdx    = 0;
    CRGB     _indicatorColor  = CRGB::Black;

    void _updateBreathing();
    void _updateRainbow();
};

extern LEDController led;
