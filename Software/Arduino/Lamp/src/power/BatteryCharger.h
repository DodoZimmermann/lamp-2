#pragma once
#include <Arduino.h>

enum class ChargeState { NOT_CHARGING, PRECHARGE, FAST_CHARGE, DONE, FAULT };

class BatteryCharger {
public:
    bool begin();
    void enableCharging(bool en);
    ChargeState getState();
    bool        hasFault();
    void        printStatus();

private:
    // BQ25303J register map (verify addresses against datasheet)
    static constexpr uint8_t REG_STATUS = 0x08;
    static constexpr uint8_t REG_FAULT  = 0x09;
};

extern BatteryCharger charger;
