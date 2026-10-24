#ifndef ARDUINOEEPROMSTORAGE_H
#define ARDUINOEEPROMSTORAGE_H

#include "BaseEepromStorage.h"

#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
#include <EEPROM.h>
#endif

class ArduinoEepromStorage : public BaseEepromStorage {
public:
    ArduinoEepromStorage();

    uint8_t read(unsigned int addr) override;
    void write(unsigned int addr, uint8_t val) override;
};

#endif // ARDUINOEEPROMSTORAGE_H
