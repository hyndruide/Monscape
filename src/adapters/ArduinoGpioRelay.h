#ifndef ARDUINOGPIORELAY_H
#define ARDUINOGPIORELAY_H

#include "../ports/IActuatorPort.h"

#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
#include <Arduino.h>
#endif

class ArduinoGpioRelay : public IActuatorPort {
private:
    char _pinRelai[4];
    int _numRelais;
    bool _states[4];

public:
    ArduinoGpioRelay(char pin);
    ArduinoGpioRelay(const char pins[], int count);

    void init() override;
    void activate(int index = 0) override;
    void deactivate(int index = 0) override;
    void toggle(int index = 0) override;
    bool isActive(int index = 0) const override;
    int getCount() const override { return _numRelais; }
    char getPin(int index = 0) const { return (index >= 0 && index < _numRelais) ? _pinRelai[index] : 0; }
};

#endif // ARDUINOGPIORELAY_H
