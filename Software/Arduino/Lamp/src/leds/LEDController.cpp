#include "LEDController.h"
#include <math.h>

LEDController led;

void LEDController::begin() {
    FastLED.addLeds<WS2812B, PIN_LED_DATA, GRB>(_leds, LED_COUNT)
           .setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(map(_brightness, 0, 100, 0, 255));
    fill_solid(_leds, LED_COUNT, CRGB::Black);
    FastLED.show();
}

void LEDController::setBrightness(uint8_t pct) {
    _brightness = constrain(pct, 0, 100);
    FastLED.setBrightness(map(_brightness, 0, 100, 0, 255));
}

void LEDController::setColor(uint8_t r, uint8_t g, uint8_t b) {
    _color = CRGB(r, g, b);
}

void LEDController::setEffect(Effect e, uint16_t speedMs) {
    _effect  = e;
    _speedMs = speedMs;

    if (e == Effect::OFF) {
        fill_solid(_leds, LED_COUNT, CRGB::Black);
        FastLED.show();
    }
}

void LEDController::update() {
    switch (_effect) {
        case Effect::OFF:     break;
        case Effect::SOLID:   FastLED.setBrightness(map(_brightness, 0, 100, 0, 255));
                              fill_solid(_leds, LED_COUNT, _color);
                              FastLED.show();
                              break;
        case Effect::BREATHING:  _updateBreathing(); break;
        case Effect::RAINBOW:    _updateRainbow();   break;
    }
}

void LEDController::_updateBreathing() {
    uint32_t t     = millis() % _speedMs;
    float    phase = (float)t / _speedMs * TWO_PI;
    float    scale = (sinf(phase - HALF_PI) + 1.0f) / 2.0f; // 0..1, starts at 0
    uint8_t  bright = (uint8_t)(map(_brightness, 0, 100, 0, 255) * scale);
    FastLED.setBrightness(bright);
    fill_solid(_leds, LED_COUNT, _color);
    FastLED.show();
}

void LEDController::_updateRainbow() {
    FastLED.setBrightness(map(_brightness, 0, 100, 0, 255));
    uint8_t startHue = (uint8_t)((millis() % (uint32_t)_speedMs) * 255UL / _speedMs);
    fill_rainbow(_leds, LED_COUNT, startHue, 255 / LED_COUNT);
    FastLED.show();
}
