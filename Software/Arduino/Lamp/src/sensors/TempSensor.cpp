#include "TempSensor.h"
#include "../system/Config.h"
#include "../system/I2CManager.h"

TempSensor tempSensor;

bool TempSensor::begin() {
    uint16_t devId;
    if (!i2c.readWord(I2C_ADDR_TMP1075, REG_DEV_ID, devId)) {
        Serial.println("[TMP] TMP1075 not found");
        return false;
    }
    if ((devId >> 8) != EXPECTED_DEV_ID) {
        Serial.printf("[TMP] Unexpected device ID: 0x%04X\n", devId);
        return false;
    }

    // CONFIG is a 16-bit register (big-endian). Bit layout:
    //   D15=OS  D14:D13=R[1:0]  D12:D11=F[1:0]  D10=POL  D9=TM  D8=SD  D7:D0=reserved
    // 0x4000 → R=10 (110ms), F=00 (1 fault), POL=0 (alert LOW), TM=0 (comparator), SD=0 (continuous)
    i2c.writeWord(I2C_ADDR_TMP1075, REG_CONFIG, 0x4000);

    setAlertThresholds(TEMP_ALERT_LOW_C, TEMP_ALERT_HIGH_C);

    Serial.println("[TMP] TMP1075 ready");
    return true;
}

float TempSensor::getTemperature() {
    uint16_t raw;
    if (!i2c.readWord(I2C_ADDR_TMP1075, REG_TEMP, raw)) return NAN;

    // Upper 12 bits, 2s-complement, 0.0625°C/LSB
    int16_t signed_raw = (int16_t)raw >> 4;
    return signed_raw * 0.0625f;
}

bool TempSensor::alertTriggered() {
    if (!_alertFlag) return false;
    _alertFlag = false;
    return true;
}

void TempSensor::handleAlert() {
    _alertFlag = true;
}

bool TempSensor::setAlertThresholds(float lowC, float highC) {
    bool ok = true;
    ok &= i2c.writeWord(I2C_ADDR_TMP1075, REG_T_LOW,  encodeTemp(lowC));
    ok &= i2c.writeWord(I2C_ADDR_TMP1075, REG_T_HIGH, encodeTemp(highC));
    return ok;
}

uint16_t TempSensor::encodeTemp(float tempC) {
    // Encode as 12-bit 2s-complement in upper bits (same format as REG_TEMP)
    int16_t raw = (int16_t)(tempC / 0.0625f);
    return (uint16_t)(raw << 4);
}
