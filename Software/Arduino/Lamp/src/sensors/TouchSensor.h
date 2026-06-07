#pragma once
#include <Arduino.h>

class TouchSensor {
public:
    using TouchCallback = void (*)(uint8_t padMask);

    bool begin();
    uint8_t getTouchedPads();              // Returns bitmask of active pads (bits 5:0); clears INT
    void    setCallback(TouchCallback cb);
    void    handleAlert();                 // Call from ALERT_TOUCH ISR flag

private:
    void clearInterrupt();

    // CAP1296 register map
    static constexpr uint8_t REG_MAIN_CTRL     = 0x00;
    static constexpr uint8_t REG_GEN_STATUS    = 0x02;
    static constexpr uint8_t REG_TOUCH_STATUS  = 0x03;
    static constexpr uint8_t REG_SENSITIVITY   = 0x1F;
    static constexpr uint8_t REG_INTR_ENABLE   = 0x27;
    static constexpr uint8_t REG_REPEAT_RATE   = 0x28;
    static constexpr uint8_t REG_PRODUCT_ID    = 0xFD;

    static constexpr uint8_t EXPECTED_PRODUCT_ID = 0x69;  // CAP1296

    TouchCallback _callback = nullptr;
    volatile bool _alertFlag = false;
};

extern TouchSensor touchSensor;
