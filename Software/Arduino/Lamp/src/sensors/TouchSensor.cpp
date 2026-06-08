#include "TouchSensor.h"
#include "../system/Config.h"
#include "../system/I2CManager.h"

TouchSensor touchSensor;

bool TouchSensor::begin() {
    uint8_t pid;
    if (!i2c.readByte(I2C_ADDR_CAP1296, REG_PRODUCT_ID, pid)) {
        printf("[TOUCH] CAP1296 not found\n");
        return false;
    }
    if (pid != EXPECTED_PRODUCT_ID) {
        printf("[TOUCH] Unexpected product ID: 0x%02X\n", pid);
        return false;
    }

    // Sensitivity: 32x (bits [6:4] = 001; higher number = more sensitive)
    i2c.writeByte(I2C_ADDR_CAP1296, REG_SENSITIVITY, 0x2F);

    // Enable interrupts for all 6 pads
    i2c.writeByte(I2C_ADDR_CAP1296, REG_INTR_ENABLE, 0x3F);

    // Disable key repeat to avoid holding-touch spam
    i2c.writeByte(I2C_ADDR_CAP1296, REG_REPEAT_RATE, 0x00);

    printf("[TOUCH] CAP1296 ready\n");
    return true;
}

uint8_t TouchSensor::getTouchedPads() {
    uint8_t status;
    if (!i2c.readByte(I2C_ADDR_CAP1296, REG_TOUCH_STATUS, status)) return 0;
    clearInterrupt();
    return status & 0x3F;  // bits 5:0 = pads 1–6
}

void TouchSensor::setCallback(TouchCallback cb) {
    _callback = cb;
}

void TouchSensor::handleAlert() {
    _alertFlag = true;
}

void TouchSensor::clearInterrupt() {
    // Clear INT bit (bit 0) in Main Control register without changing other bits
    uint8_t ctrl;
    if (i2c.readByte(I2C_ADDR_CAP1296, REG_MAIN_CTRL, ctrl))
        i2c.writeByte(I2C_ADDR_CAP1296, REG_MAIN_CTRL, ctrl & ~0x01);
}
