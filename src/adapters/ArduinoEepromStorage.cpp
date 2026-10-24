#include "ArduinoEepromStorage.h"

ArduinoEepromStorage::ArduinoEepromStorage() {
#if defined(ESP32)
    EEPROM.begin(MONSCAPE_EEPROM_TOTAL_SIZE);
#endif
}

uint8_t ArduinoEepromStorage::read(unsigned int addr) {
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
    return EEPROM.read(addr);
#else
    (void)addr;
    return 0;
#endif
}

void ArduinoEepromStorage::write(unsigned int addr, uint8_t val) {
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
    EEPROM.write(addr, val);
#if defined(ESP32)
    EEPROM.commit();
#endif
#else
    (void)addr;
    (void)val;
#endif
}
