#ifndef ARDUINORS485TRANSPORT_H
#define ARDUINORS485TRANSPORT_H

#include "../ports/ITransportPort.h"

#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
#include <Arduino.h>
#if !defined(ESP32)
#include <SoftwareSerial.h>
#endif
#endif

class ArduinoRS485Transport : public ITransportPort {
private:
    byte _pinRS;
    int _pinRx;
    int _pinTx;
    int _baud;
#if (defined(ARDUINO) && ARDUINO >= 100) && !defined(ESP32)
    SoftwareSerial* _softwareSerial;
#endif

public:
    ArduinoRS485Transport(byte pinRS = 0);
    virtual ~ArduinoRS485Transport();

    void setRsPin(byte pinRS);
    bool begin(int rxPin, int txPin, int baud) override;
    bool send(const MString& data) override;
    bool available() override;
    MString readStringUntil(char delimiter = '\n') override;
    void clear() override;
};

#endif // ARDUINORS485TRANSPORT_H
