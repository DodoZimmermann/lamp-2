#include "I2CManager.h"

I2CManager i2c;

bool I2CManager::begin(int sda, int scl, uint32_t freq) {
    return Wire.begin(sda, scl, freq);
}

void I2CManager::scan() {
    printf("[I2C] Scanning...\n");
    uint8_t found = 0;
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            printf("[I2C]   0x%02X\n", addr);
            found++;
        }
    }
    printf("[I2C] %u device(s)\n", found);
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
    val = ((uint16_t)Wire.read() << 8) | Wire.read();
    return true;
}

bool I2CManager::writeWord(uint8_t addr, uint8_t reg, uint16_t val) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write((val >> 8) & 0xFF);
    Wire.write(val & 0xFF);
    return Wire.endTransmission() == 0;
}
