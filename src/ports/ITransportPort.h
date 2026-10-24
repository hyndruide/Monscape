#ifndef ITRANSPORTPORT_H
#define ITRANSPORTPORT_H

#include "../domain/GameState.h"

class ITransportPort {
public:
    virtual ~ITransportPort() {}

    virtual bool begin(int rxPin, int txPin, int baud) = 0;
    virtual bool send(const MString& data) = 0;
    virtual bool available() = 0;
    virtual MString readStringUntil(char delimiter = '\n') = 0;
    virtual void clear() = 0;
};

#endif // ITRANSPORTPORT_H
