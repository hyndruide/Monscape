#ifndef MOCKSTORAGE_H
#define MOCKSTORAGE_H

#include "BaseEepromStorage.h"
#include <vector>

class MockStorage : public BaseEepromStorage {
private:
    uint8_t _buffer[MONSCAPE_EEPROM_TOTAL_SIZE];

public:
    MockStorage() {
        reset();
    }

    void reset() {
        for (int i = 0; i < MONSCAPE_EEPROM_TOTAL_SIZE; i++) {
            _buffer[i] = 17; // formatted default
        }
    }

    uint8_t read(unsigned int addr) override {
        if (addr < MONSCAPE_EEPROM_TOTAL_SIZE) {
            return _buffer[addr];
        }
        return 0;
    }

    void write(unsigned int addr, uint8_t val) override {
        if (addr < MONSCAPE_EEPROM_TOTAL_SIZE) {
            _buffer[addr] = val;
        }
    }
};

#endif // MOCKSTORAGE_H
