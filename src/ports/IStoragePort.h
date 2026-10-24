#ifndef ISTORAGEPORT_H
#define ISTORAGEPORT_H

#include "../domain/GameState.h"

class IStoragePort {
public:
    virtual ~IStoragePort() {}

    virtual uint8_t read(unsigned int addr) = 0;
    virtual void write(unsigned int addr, uint8_t val) = 0;
    virtual void format() = 0;
    virtual bool isSlotEmpty(unsigned int addr) = 0;
    virtual void writeData(const MString& data) = 0;
    virtual MString readString(unsigned int addr) = 0;
    virtual unsigned char readChar(unsigned int addr) = 0;
    virtual int readInt(unsigned int addr) = 0;
    virtual void readHeader(char* value) = 0;
    virtual MString parseSlot(unsigned int addr) = 0;
    virtual MString sendCommandFromSlot(unsigned int addr) = 0;
};

#endif // ISTORAGEPORT_H
