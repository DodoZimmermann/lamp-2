#include "USBPDController.h"
#include "../system/Config.h"
#include "../system/I2CManager.h"

USBPDController usbPD;

bool USBPDController::begin() {
    delay(200);  // allow CYPD3177 firmware to complete boot
    clearInterrupt();

    if (!configurePDO15V3A()) {
        Serial.println("[PD] PDO 15V/3A request failed — verify charger supports 15V");
        return false;
    }
    Serial.println("[PD] CYPD3177: 15V/3A PDO requested");
    return true;
}

bool USBPDController::isConnected() {
    return digitalRead(PIN_PG_PD) == HIGH;
}

void USBPDController::handleInterrupt() {
    uint8_t flags;
    if (i2c.readHPI(I2C_ADDR_CYPD3177, HPI_INTR_REG, &flags, 1)) {
        Serial.printf("[PD] Interrupt flags: 0x%02X\n", flags);
    }
    clearInterrupt();
}

bool USBPDController::configurePDO15V3A() {
    // Write PDO index to SELECT_SINK_PDO (0x1005).
    // PDO_INDEX_15V = 0x03 assumes the charger advertises: 0=5V, 1=9V, 2=12V, 3=15V.
    // If the charger skips 12V, 15V may be at index 2. Use a USB-PD analyzer to confirm.
    uint8_t pdo = PDO_INDEX_15V;
    if (!i2c.writeHPI(I2C_ADDR_CYPD3177, HPI_SELECT_SINK_PDO, &pdo, 1)) {
        return false;
    }

    delay(100);  // allow negotiation to start

    uint8_t response[2];
    if (i2c.readHPI(I2C_ADDR_CYPD3177, HPI_PD_RESPONSE, response, 2)) {
        Serial.printf("[PD] HPI response: 0x%02X 0x%02X%s\n",
                      response[0], response[1],
                      response[0] == 0x02 ? " (success)" : " (check PDO index)");
    }
    return true;
}

void USBPDController::clearInterrupt() {
    uint8_t clear = 0xFF;
    i2c.writeHPI(I2C_ADDR_CYPD3177, HPI_INTR_REG, &clear, 1);
}
