#include "ArduinoGpioRelay.h"

ArduinoGpioRelay::ArduinoGpioRelay(char pin) : _numRelais(1) {
    _pinRelai[0] = pin;
    for (int i = 0; i < 4; i++) _states[i] = false;
    init();
}

ArduinoGpioRelay::ArduinoGpioRelay(const char pins[], int count) : _numRelais(count > 4 ? 4 : count) {
    for (int i = 0; i < _numRelais; i++) {
        _pinRelai[i] = pins[i];
    }
    for (int i = 0; i < 4; i++) _states[i] = false;
    init();
}

void ArduinoGpioRelay::init() {
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
    for (int i = 0; i < _numRelais; i++) {
        pinMode(_pinRelai[i], OUTPUT);
        digitalWrite(_pinRelai[i], LOW);
    }
#endif
    for (int i = 0; i < 4; i++) _states[i] = false;
}

void ArduinoGpioRelay::activate(int index) {
    if (index >= 0 && index < _numRelais) {
        _states[index] = true;
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
        digitalWrite(_pinRelai[index], HIGH);
#endif
    }
}

void ArduinoGpioRelay::deactivate(int index) {
    if (index >= 0 && index < _numRelais) {
        _states[index] = false;
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
        digitalWrite(_pinRelai[index], LOW);
#endif
    }
}

void ArduinoGpioRelay::toggle(int index) {
    if (index >= 0 && index < _numRelais) {
        if (_states[index]) {
            deactivate(index);
        } else {
            activate(index);
        }
    }
}

bool ArduinoGpioRelay::isActive(int index) const {
    if (index >= 0 && index < _numRelais) {
        return _states[index];
    }
    return false;
}
