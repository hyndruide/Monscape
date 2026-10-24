#ifndef MOCKACTUATOR_H
#define MOCKACTUATOR_H

#include "../ports/IActuatorPort.h"

class MockActuator : public IActuatorPort {
private:
    bool _states[4];
    int _count;

public:
    MockActuator(int count = 1) : _count(count) {
        for (int i = 0; i < 4; i++) _states[i] = false;
    }

    void init() override {
        for (int i = 0; i < 4; i++) _states[i] = false;
    }

    void activate(int index = 0) override {
        if (index >= 0 && index < 4) _states[index] = true;
    }

    void deactivate(int index = 0) override {
        if (index >= 0 && index < 4) _states[index] = false;
    }

    void toggle(int index = 0) override {
        if (index >= 0 && index < 4) _states[index] = !_states[index];
    }

    bool isActive(int index = 0) const override {
        if (index >= 0 && index < 4) return _states[index];
        return false;
    }

    int getCount() const override {
        return _count;
    }
};

#endif // MOCKACTUATOR_H
