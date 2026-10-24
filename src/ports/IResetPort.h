#ifndef IRESETPORT_H
#define IRESETPORT_H

class IResetPort {
public:
    virtual ~IResetPort() {}
    virtual void reset() = 0;
};

#endif // IRESETPORT_H
