#include "BatteryCharger.h"
#include "../system/Config.h"
#include "../system/I2CManager.h"

BatteryCharger charger;

bool BatteryCharger::begin() {
    // Charging on/off is controlled by PIN_EN_CHG GPIO (see Lamp.ino power task).
    // I2C is used only for status readout.
    uint8_t status;
    if (!i2c.readByte(I2C_ADDR_BQ25303J, REG_STATUS, status)) {
        Serial.println("[CHG] BQ25303J not found");
        return false;
    }
    Serial.printf("[CHG] Status reg: 0x%02X\n", status);
    return true;
}

void BatteryCharger::enableCharging(bool en) {
    // Physical enable/disable via active-HIGH EN_CHG pin
    digitalWrite(PIN_EN_CHG, en ? HIGH : LOW);
}

ChargeState BatteryCharger::getState() {
    uint8_t status;
    if (!i2c.readByte(I2C_ADDR_BQ25303J, REG_STATUS, status)) return ChargeState::FAULT;

    // Bits [5:4] = CHRG_STAT
    switch ((status >> 4) & 0x03) {
        case 0x00: return ChargeState::NOT_CHARGING;
        case 0x01: return ChargeState::PRECHARGE;
        case 0x02: return ChargeState::FAST_CHARGE;
        case 0x03: return ChargeState::DONE;
    }
    return ChargeState::NOT_CHARGING;
}

bool BatteryCharger::hasFault() {
    uint8_t fault;
    if (!i2c.readByte(I2C_ADDR_BQ25303J, REG_FAULT, fault)) return true;
    return fault != 0x00;
}

void BatteryCharger::printStatus() {
    static const char *stateNames[] = {"Not Charging", "Pre-charge", "Fast Charge", "Done", "Fault"};
    ChargeState s = getState();
    uint8_t fault;
    i2c.readByte(I2C_ADDR_BQ25303J, REG_FAULT, fault);
    Serial.printf("[CHG] State: %s | Fault reg: 0x%02X\n",
                  stateNames[(int)s], fault);
}
