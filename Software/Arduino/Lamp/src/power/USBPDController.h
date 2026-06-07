#pragma once
#include <Arduino.h>

class USBPDController {
public:
    bool begin();                  // Configure CYPD3177 for 15V/3A at boot
    bool isConnected();            // Check PG_PD pin
    void handleInterrupt();        // Call from PD_INT ISR flag

private:
    bool configurePDO15V3A();
    void clearInterrupt();

    // CYPD3177 HPI register map — verified against ANAVI CYPD3177 example sketch
    static constexpr uint16_t HPI_INTR_REG          = 0x0006;  // interrupt flags; write 0xFF to clear
    static constexpr uint16_t HPI_SELECT_SINK_PDO   = 0x1005;  // write PDO index byte to select profile
    static constexpr uint16_t HPI_PD_STATUS         = 0x1008;  // 4 bytes: negotiation status
    static constexpr uint16_t HPI_PD_RESPONSE       = 0x1400;  // 2 bytes: response from last command (0x02=success)

    // PDO index for 15V (0-based: 0=5V, 1=9V, 2=12V, 3=15V — typical GaN charger ordering)
    // If 15V is not negotiated, check actual PDO order with a USB-PD analyzer
    static constexpr uint8_t PDO_INDEX_15V = 0x03;
};

extern USBPDController usbPD;
