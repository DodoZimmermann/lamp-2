#pragma once
#include <Arduino.h>
#include <Wire.h>

class I2CManager {
public:
    bool begin(int sda, int scl, uint32_t freq = 400000);
    void scan();

    bool readByte (uint8_t addr, uint8_t reg, uint8_t  &val);
    bool writeByte(uint8_t addr, uint8_t reg, uint8_t   val);
    bool readWord (uint8_t addr, uint8_t reg, uint16_t &val);
    bool writeWord(uint8_t addr, uint8_t reg, uint16_t  val);
};

extern I2CManager i2c;
