#ifndef PUZZLEENGINE_H
#define PUZZLEENGINE_H

#include "GameState.h"
#include "../ports/ITransportPort.h"
#include "../ports/IStoragePort.h"
#include "../ports/IActuatorPort.h"
#include "../ports/IResetPort.h"

class PuzzleEngine {
private:
    MString _nomSys;
    MString _adresse;
    MString _verG;
    MString _verMs;
    MString _descGame;
    MString _winCode;
    Trame _trame;
    byte _protocole;

    ITransportPort* _transport;
    IStoragePort* _storage;
    IActuatorPort* _actuator;
    IResetPort* _reset;

public:
    PuzzleEngine(byte protocole = MSCape_RS485,
                 ITransportPort* transport = nullptr,
                 IStoragePort* storage = nullptr,
                 IActuatorPort* actuator = nullptr,
                 IResetPort* reset = nullptr);

    // Dependency injection
    void setTransportPort(ITransportPort* transport);
    void setStoragePort(IStoragePort* storage);
    void setActuatorPort(IActuatorPort* actuator);
    void setResetPort(IResetPort* reset);

    ITransportPort* getTransportPort() const { return _transport; }
    IStoragePort* getStoragePort() const { return _storage; }
    IActuatorPort* getActuatorPort() const { return _actuator; }
    IResetPort* getResetPort() const { return _reset; }

    // Configuration
    bool descGame(const MString& nomSys, const MString& adresse, const MString& verG,
                  const MString& descGame, const MString& winCode);
    void setWin(const MString& winCode);
    MString getWinCode() const { return _winCode; }

    MString getNomSys() const { return _nomSys; }
    MString getAdresse() const { return _adresse; }
    MString getVerG() const { return _verG; }
    MString getVerMs() const { return _verMs; }
    MString getDescGame() const { return _descGame; }
    byte getProtocole() const { return _protocole; }
    void setProtocole(byte protocole) { _protocole = protocole; }

    // State & Trame management
    void setTrameStat(char stat);
    char getTrameStat() const { return _trame.stat; }
    void setTrameInput(const MString& input);
    void setTrameLastInput(const MString& lastInput);
    Trame getTrame() const { return _trame; }

    // Game logic
    bool win(bool bypass = false);
    bool fail();
    bool checkWin(const MString& code);

    // Serialization & Protocol
    MString parseCommande(const MString& value);
    MString codeCommande(int type, const char* adru, char cmd, char data);
    MString codeCommande(int type, const char* adru, char cmd, const MString& data);
    MString codeCommande(int type, const char* adru, char cmd, const char* data);

    MString buildInitTrameJson() const;
    MString buildLogTrameJson() const;
    MString buildSendTrameJson(const MString& to, const MString& command) const;

    // Command handling
    CommandAction evaluateCommand(const MString& targetAddress, char commandChar) const;
    CommandAction processIncomingJson(const MString& jsonString);

    // High level workflow
    bool sendTrame(const MString& to, const MString& command);
    bool logTrame();
    bool initTrame();
    bool listenServ();
};

#endif // PUZZLEENGINE_H
