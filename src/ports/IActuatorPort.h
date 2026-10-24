#ifndef IACTUATORPORT_H
#define IACTUATORPORT_H

class IActuatorPort {
public:
    virtual ~IActuatorPort() {}

    virtual void init() = 0;
    virtual void activate(int index = 0) = 0;
    virtual void deactivate(int index = 0) = 0;
    virtual void toggle(int index = 0) = 0;
    virtual bool isActive(int index = 0) const = 0;
    virtual int getCount() const = 0;
};

#endif // IACTUATORPORT_H
