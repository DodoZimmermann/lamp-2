#include "BatteryCharger.h"
#include "../system/Config.h"

BatteryCharger charger;

void BatteryCharger::begin() {
    pinMode(PIN_EN_CHG, OUTPUT);
    digitalWrite(PIN_EN_CHG, LOW);
}

void BatteryCharger::enable(bool en) {
    digitalWrite(PIN_EN_CHG, en ? HIGH : LOW);
}
