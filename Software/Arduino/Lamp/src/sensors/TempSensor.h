#pragma once
#include <Arduino.h>

class TempSensor {
public:
    bool  begin();
    float getTemperature();        // Returns °C; NAN on error
    bool  alertTriggered();        // Check and clear alert flag
    void  handleAlert();           // Call from ALERT_TMP ISR flag

private:
    bool setAlertThresholds(float lowC, float highC);
    uint16_t encodeTemp(float tempC);

    // TMP1075 register map
    static constexpr uint8_t REG_TEMP    = 0x00;
    static constexpr uint8_t REG_CONFIG  = 0x01;
    static constexpr uint8_t REG_T_LOW   = 0x02;
    static constexpr uint8_t REG_T_HIGH  = 0x03;
    static constexpr uint8_t REG_DEV_ID  = 0x0F;

    static constexpr uint8_t EXPECTED_DEV_ID = 0x75;

    volatile bool _alertFlag = false;
};

extern TempSensor tempSensor;
