#ifndef MOCKTRANSPORT_H
#define MOCKTRANSPORT_H

#include "../ports/ITransportPort.h"
#include <vector>
#include <string>

class MockTransport : public ITransportPort {
public:
    std::vector<MString> sentMessages;
    std::vector<MString> incomingQueue;
    bool cleared;
    bool initialized;

    MockTransport() : cleared(false), initialized(false) {}

    bool begin(int rxPin, int txPin, int baud) override {
        (void)rxPin; (void)txPin; (void)baud;
        initialized = true;
        return true;
    }

    bool send(const MString& data) override {
        sentMessages.push_back(data);
        return true;
    }

    bool available() override {
        return !incomingQueue.empty();
    }

    MString readStringUntil(char delimiter = '\n') override {
        (void)delimiter;
        if (incomingQueue.empty()) return MString("");
        MString msg = incomingQueue.front();
        incomingQueue.erase(incomingQueue.begin());
        return msg;
    }

    void clear() override {
        cleared = true;
    }

    void injectIncoming(const MString& msg) {
        incomingQueue.push_back(msg);
    }

    MString getLastSent() const {
        if (sentMessages.empty()) return MString("");
        return sentMessages.back();
    }
};

#endif // MOCKTRANSPORT_H
