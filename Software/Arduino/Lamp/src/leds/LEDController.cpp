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

void LEDController::setEffect(Effect e) {
    _effect = e;
    if (e == Effect::OFF) {
        fill_solid(_leds, LED_COUNT, CRGB::Black);
        FastLED.show();
    }
}

void LEDController::setIndicator(uint8_t idx, CRGB color, bool active) {
    _indicatorActive = active;
    _indicatorIdx    = idx;
    _indicatorColor  = color;
}

void LEDController::update() {
    switch (_effect) {
        case Effect::OFF:
            break;
        case Effect::SOLID:
            FastLED.setBrightness(map(_brightness, 0, 100, 0, 255));
            fill_solid(_leds, LED_COUNT, _color);
            break;
        case Effect::BREATHING:
            _updateBreathing();
            break;
        case Effect::RAINBOW:
            _updateRainbow();
            break;
    }

    // Overlay indicator LED after effect (full brightness, ignores global dimming)
    if (_indicatorActive)
        _leds[_indicatorIdx] = _indicatorColor;

    if (_effect != Effect::OFF || _indicatorActive)
        FastLED.show();
}

void LEDController::_updateBreathing() {
    float phase = (float)(millis() % BREATHE_SPEED_MS) / BREATHE_SPEED_MS * TWO_PI;
    float scale = 0.30f + (sinf(phase - HALF_PI) + 1.0f) / 2.0f * 0.70f;  // 30–100 %
    FastLED.setBrightness((uint8_t)(map(_brightness, 0, 100, 0, 255) * scale));
    fill_solid(_leds, LED_COUNT, _color);
}

void LEDController::_updateRainbow() {
    FastLED.setBrightness(map(_brightness, 0, 100, 0, 255));
    uint8_t hue = (uint8_t)((millis() % RAINBOW_SPEED_MS) * 255UL / RAINBOW_SPEED_MS);
    fill_solid(_leds, LED_COUNT, CHSV(hue, 255, 255));
}
