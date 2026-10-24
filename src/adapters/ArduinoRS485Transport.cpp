#include "ArduinoRS485Transport.h"

ArduinoRS485Transport::ArduinoRS485Transport(byte pinRS)
    : _pinRS(pinRS), _pinRx(-1), _pinTx(-1), _baud(28800)
#if (defined(ARDUINO) && ARDUINO >= 100) && !defined(ESP32)
      , _softwareSerial(nullptr)
#endif
{
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
    if (_pinRS > 0) {
        pinMode(_pinRS, OUTPUT);
        digitalWrite(_pinRS, LOW);
    }
#endif
}

ArduinoRS485Transport::~ArduinoRS485Transport() {
#if (defined(ARDUINO) && ARDUINO >= 100) && !defined(ESP32)
    if (_softwareSerial != nullptr) {
        delete _softwareSerial;
        _softwareSerial = nullptr;
    }
#endif
}

void ArduinoRS485Transport::setRsPin(byte pinRS) {
    _pinRS = pinRS;
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
    pinMode(_pinRS, OUTPUT);
    digitalWrite(_pinRS, LOW);
#endif
}

bool ArduinoRS485Transport::begin(int rxPin, int txPin, int baud) {
    _pinRx = rxPin;
    _pinTx = txPin;
    _baud = baud;

#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
    if (_pinRS > 0) {
        pinMode(_pinRS, OUTPUT);
        digitalWrite(_pinRS, LOW);
    }
#if defined(ESP32)
    Serial.begin(115200);
    Serial.println("Communication OK");
    Serial2.begin(_baud, SERIAL_8N1, _pinRx, _pinTx);
    Serial.println("Communication RS485");
#else
    Serial.begin(115200);
    Serial.println("Communication OK");
    if (_softwareSerial != nullptr) {
        delete _softwareSerial;
    }
    _softwareSerial = new SoftwareSerial(_pinRx, _pinTx);
    _softwareSerial->begin(_baud);
    Serial.println("Communication RS485");
    Serial.println("Done!");
#endif
#endif
    return true;
}

bool ArduinoRS485Transport::send(const MString& data) {
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
    if (_pinRS > 0) digitalWrite(_pinRS, HIGH);

#if defined(ESP32)
    delay(30);
    Serial2.println(data);
    delay(data.length() * 2);
#else
    delay(1);
    if (_softwareSerial != nullptr) {
        _softwareSerial->println(data);
    }
    delay(1);
#endif

    if (_pinRS > 0) digitalWrite(_pinRS, LOW);
    Serial.println(data);
#else
    (void)data;
#endif
    return true;
}

bool ArduinoRS485Transport::available() {
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
#if defined(ESP32)
    return (Serial2.available() > 0);
#else
    return (_softwareSerial != nullptr && _softwareSerial->available() > 0);
#endif
#else
    return false;
#endif
}

MString ArduinoRS485Transport::readStringUntil(char delimiter) {
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
    if (_pinRS > 0) digitalWrite(_pinRS, LOW);
#if defined(ESP32)
    return Serial2.readStringUntil(delimiter);
#else
    if (_softwareSerial != nullptr) {
        return _softwareSerial->readStringUntil(delimiter);
    }
#endif
#else
    (void)delimiter;
#endif
    return MString("");
}

void ArduinoRS485Transport::clear() {
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
    if (_pinRS > 0) digitalWrite(_pinRS, HIGH);
#if defined(ESP32)
    delay(30);
    Serial2.println("");
    delay(100);
#else
    if (_softwareSerial != nullptr) {
        _softwareSerial->println("");
    }
#endif
    if (_pinRS > 0) digitalWrite(_pinRS, LOW);
#endif
}
