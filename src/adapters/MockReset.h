#ifndef MOCKRESET_H
#define MOCKRESET_H

#include "../ports/IResetPort.h"

class MockReset : public IResetPort {
public:
    bool resetCalled;

    MockReset() : resetCalled(false) {}

    void reset() override {
        resetCalled = true;
    }
};

#endif // MOCKRESET_H
