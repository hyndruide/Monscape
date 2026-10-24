#ifndef BASEEEPROMSTORAGE_H
#define BASEEEPROMSTORAGE_H

#include "../ports/IStoragePort.h"

class BaseEepromStorage : public IStoragePort {
protected:
    char lastEntry(const char* header);
    int emptySlot(const char* header);

public:
    virtual ~BaseEepromStorage() {}

    virtual void format() override;
    virtual bool isSlotEmpty(unsigned int addr) override;
    virtual void writeData(const MString& data) override;
    virtual MString readString(unsigned int addr) override;
    virtual unsigned char readChar(unsigned int addr) override;
    virtual int readInt(unsigned int addr) override;
    virtual void readHeader(char* value) override;
    virtual MString parseSlot(unsigned int addr) override;
    virtual MString sendCommandFromSlot(unsigned int addr) override;
};

#endif // BASEEEPROMSTORAGE_H
