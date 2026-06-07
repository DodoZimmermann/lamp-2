#pragma once
#include <Arduino.h>
#include <Wire.h>

class I2CManager {
public:
    bool begin(int sda, int scl, uint32_t freq = 400000);
    void scan();

    // Standard 8-bit register devices (TMP1075, CAP1296, BQ25303J)
    bool readByte(uint8_t addr, uint8_t reg, uint8_t &val);
    bool writeByte(uint8_t addr, uint8_t reg, uint8_t val);
    bool readWord(uint8_t addr, uint8_t reg, uint16_t &val);
    bool writeWord(uint8_t addr, uint8_t reg, uint16_t val);

    // 16-bit register address for CYPD3177 HPI protocol
    bool readHPI(uint8_t addr, uint16_t reg16, uint8_t *buf, uint8_t len);
    bool writeHPI(uint8_t addr, uint16_t reg16, const uint8_t *buf, uint8_t len);
};

extern I2CManager i2c;
