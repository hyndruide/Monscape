#ifndef ARDUINORESET_H
#define ARDUINORESET_H

#include "../ports/IResetPort.h"

#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
#include <Arduino.h>
#endif

class ArduinoReset : public IResetPort {
public:
    void reset() override {
#if defined(ESP32)
        ESP.restart();
#elif (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR)
        asm volatile ("  jmp 0");
#endif
    }
};

#endif // ARDUINORESET_H
