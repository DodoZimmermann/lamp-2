#include "I2CManager.h"

I2CManager i2c;

bool I2CManager::begin(int sda, int scl, uint32_t freq) {
    return Wire.begin(sda, scl, freq);
}

void I2CManager::scan() {
    Serial.println("[I2C] Scanning bus...");
    uint8_t found = 0;
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.printf("[I2C]   0x%02X\n", addr);
            found++;
        }
    }
    Serial.printf("[I2C] %u device(s) found\n", found);
}

bool I2CManager::readByte(uint8_t addr, uint8_t reg, uint8_t &val) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) return false;
    if (Wire.requestFrom(addr, (uint8_t)1) != 1) return false;
    val = Wire.read();
    return true;
}

bool I2CManager::writeByte(uint8_t addr, uint8_t reg, uint8_t val) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(val);
    return Wire.endTransmission() == 0;
}

bool I2CManager::readWord(uint8_t addr, uint8_t reg, uint16_t &val) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) return false;
    if (Wire.requestFrom(addr, (uint8_t)2) != 2) return false;
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    val = ((uint16_t)msb << 8) | lsb;
    return true;
}

bool I2CManager::writeWord(uint8_t addr, uint8_t reg, uint16_t val) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write((val >> 8) & 0xFF);
    Wire.write(val & 0xFF);
    return Wire.endTransmission() == 0;
}

// CYPD3177 HPI uses 16-bit register addresses (little-endian on wire)
bool I2CManager::readHPI(uint8_t addr, uint16_t reg16, uint8_t *buf, uint8_t len) {
    Wire.beginTransmission(addr);
    Wire.write(reg16 & 0xFF);
    Wire.write((reg16 >> 8) & 0xFF);
    if (Wire.endTransmission(false) != 0) return false;
    if (Wire.requestFrom(addr, len) != len) return false;
    for (uint8_t i = 0; i < len; i++) buf[i] = Wire.read();
    return true;
}

bool I2CManager::writeHPI(uint8_t addr, uint16_t reg16, const uint8_t *buf, uint8_t len) {
    Wire.beginTransmission(addr);
    Wire.write(reg16 & 0xFF);
    Wire.write((reg16 >> 8) & 0xFF);
    for (uint8_t i = 0; i < len; i++) Wire.write(buf[i]);
    return Wire.endTransmission() == 0;
}
