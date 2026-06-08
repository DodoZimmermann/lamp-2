#pragma once
#include <Arduino.h>

class BatteryCharger {
public:
    void begin();
    void enable(bool en);
};

extern BatteryCharger charger;
