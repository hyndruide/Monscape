#include "PuzzleEngine.h"

PuzzleEngine::PuzzleEngine(byte protocole,
                           ITransportPort* transport,
                           IStoragePort* storage,
                           IActuatorPort* actuator,
                           IResetPort* reset)
    : _verMs("0.2"),
      _protocole(protocole),
      _transport(transport),
      _storage(storage),
      _actuator(actuator),
      _reset(reset) {
    _trame.stat = MSC_IDLE;
    _trame.input = "";
    _trame.last_input = "";
}

void PuzzleEngine::setTransportPort(ITransportPort* transport) {
    _transport = transport;
}

void PuzzleEngine::setStoragePort(IStoragePort* storage) {
    _storage = storage;
}

void PuzzleEngine::setActuatorPort(IActuatorPort* actuator) {
    _actuator = actuator;
}

void PuzzleEngine::setResetPort(IResetPort* reset) {
    _reset = reset;
}

bool PuzzleEngine::descGame(const MString& nomSys, const MString& adresse, const MString& verG,
                           const MString& descGame, const MString& winCode) {
    _nomSys = nomSys;
    _adresse = adresse;
    _verG = verG;
    _descGame = descGame;
    _winCode = winCode;
    return true;
}

void PuzzleEngine::setWin(const MString& winCode) {
    _winCode = winCode;
}

void PuzzleEngine::setTrameStat(char stat) {
    _trame.stat = stat;
}

void PuzzleEngine::setTrameInput(const MString& input) {
    _trame.input = input;
}

void PuzzleEngine::setTrameLastInput(const MString& lastInput) {
    _trame.last_input = lastInput;
}

bool PuzzleEngine::checkWin(const MString& code) {
    return (_winCode.length() > 0 && code == _winCode);
}

bool PuzzleEngine::win(bool bypass) {
    if (bypass) {
        setTrameStat(MSC_BYPASS);
    } else {
        setTrameStat(MSC_WIN);
    }

    if (_actuator != nullptr) {
        _actuator->activate(0);
    }

    if (_storage != nullptr) {
        for (int ko = 0; ko < MONSCAPE_EEPROM_TOTAL_SIZE; ko += MONSCAPE_EEPROM_SLOT_SIZE) {
            if (_storage->isSlotEmpty(ko)) {
                sendTrame("msc", _storage->sendCommandFromSlot(ko));
            }
        }
    }

    logTrame();
    return true;
}

bool PuzzleEngine::fail() {
    setTrameStat(MSC_BUZY);
    if (_actuator != nullptr) {
        _actuator->deactivate(0);
    }
    logTrame();
    return true;
}

MString PuzzleEngine::codeCommande(int type, const char* adru, char cmd, char data) {
    MString eepvle = MString(type);
    eepvle = eepvle + MString(adru);
    eepvle = eepvle + MString(cmd);
    eepvle = eepvle + MString("1");
    eepvle = eepvle + MString((char) 1);
    eepvle = eepvle + MString(data);
    return eepvle;
}

MString PuzzleEngine::codeCommande(int type, const char* adru, char cmd, const MString& data) {
    MString eepvle = MString(type);
    eepvle = eepvle + MString(adru);
    eepvle = eepvle + MString(cmd);
    eepvle = eepvle + MString("2");
    eepvle = eepvle + MString((char) data.length());
    eepvle = eepvle + data;
    return eepvle;
}

MString PuzzleEngine::codeCommande(int type, const char* adru, char cmd, const char* data) {
    MString eepvle = MString(type);
    eepvle = eepvle + MString(adru);
    eepvle = eepvle + MString(cmd);
    eepvle = eepvle + MString("3");
    size_t len = data ? strlen(data) : 0;
    eepvle = eepvle + MString((char) len);
    eepvle = eepvle + MString(data ? data : "");
    return eepvle;
}

MString PuzzleEngine::parseCommande(const MString& value) {
    if (value.length() < 7) return MString("{}");

    MString parse_data = MString("{\"Type\":") + value[0] +
                         MString(",\"To\":\"") + value[1] + value[2] + value[3] +
                         MString("\",\"Cmd\":\"") + value[4] + MString("\",");

    int dataType = (int) (value[5] - '0');
    switch (dataType) {
        case VARIABLE:
            parse_data = parse_data + MString("\"Data\":") + MString((unsigned char) value[7]);
            break;
        case STRING: {
            parse_data = parse_data + MString("\"Data\":\"");
            int len = (unsigned char) value[6];
            for (int i = 0; i < len && (7 + i) < (int) value.length(); i++) {
                parse_data = parse_data + value[7 + i];
            }
            parse_data = parse_data + MString("\"");
            break;
        }
        case ARRAY: {
            parse_data = parse_data + MString("\"Data\":[");
            int len = (unsigned char) value[6];
            for (int i = 0; i < len && (7 + i) < (int) value.length(); i++) {
                parse_data = parse_data + MString((unsigned char) value[7 + i]);
                parse_data = parse_data + MString(",");
            }
            if (len > 0) {
                parse_data = parse_data.substring(0, parse_data.length() - 1);
            }
            parse_data = parse_data + MString("]");
            break;
        }
        default:
            break;
    }
    parse_data = parse_data + MString("}");
    return parse_data;
}

MString PuzzleEngine::buildInitTrameJson() const {
    MString json = MString("{\"Nom\":\"") + _nomSys +
                   MString("\",\"Adr\":\"") + _adresse +
                   MString("\",\"V_G\":\"") + _verG +
                   MString("\",\"V_mon\":\"") + _verMs +
                   MString("\",\"Desc\":\"") + _descGame +
                   MString("\",\"Win\":\"") + _winCode +
                   MString("\"}");
    return json;
}

MString PuzzleEngine::buildLogTrameJson() const {
    MString json = MString("{\"Nom\":\"") + _nomSys +
                   MString("\",\"Adr\":\"") + _adresse +
                   MString("\",\"stat\":") + MString((int) _trame.stat) +
                   MString("}");
    return json;
}

MString PuzzleEngine::buildSendTrameJson(const MString& to, const MString& command) const {
    MString json = MString("{\"Adr\":\"") + _adresse +
                   MString("\",\"A\":\"") + to +
                   MString("\",\"C\":\"S\",\"exec\":\"") + command +
                   MString("\"}");
    return json;
}

CommandAction PuzzleEngine::evaluateCommand(const MString& targetAddress, char commandChar) const {
    if (targetAddress != _adresse) {
        return ACTION_NONE;
    }

    switch (commandChar) {
        case 'R': return ACTION_RESET;
        case 'B': return ACTION_BYPASS;
        case 'X': return ACTION_SEND_EEPROM;
        case 'E': return ACTION_LOAD_EEPROM;
        case 'S': return ACTION_SPECIAL;
        case 'I': return ACTION_LOG_TRAME;
        default:  return ACTION_NONE;
    }
}

static MString extractField(const MString& json, const char* field) {
    MString key1 = MString("\"") + field + MString("\":\"");
    size_t pos = json.find(key1.c_str());
    if (pos != std::string::npos) {
        size_t start = pos + key1.length();
        size_t end = json.find('"', start);
        if (end != std::string::npos) {
            return json.substring(start, end);
        }
    }
    MString key2 = MString("\"") + field + MString("\": \"");
    pos = json.find(key2.c_str());
    if (pos != std::string::npos) {
        size_t start = pos + key2.length();
        size_t end = json.find('"', start);
        if (end != std::string::npos) {
            return json.substring(start, end);
        }
    }
    return MString("");
}

CommandAction PuzzleEngine::processIncomingJson(const MString& jsonString) {
    MString targetAdr = extractField(jsonString, "A");
    MString cmdStr = extractField(jsonString, "C");
    if (cmdStr.length() == 0) return ACTION_NONE;

    char cmdChar = cmdStr[0];
    CommandAction action = evaluateCommand(targetAdr, cmdChar);

    switch (action) {
        case ACTION_RESET:
            if (_reset != nullptr) {
                _reset->reset();
            }
            break;
        case ACTION_BYPASS:
            win(true);
            break;
        case ACTION_SEND_EEPROM:
            if (_storage != nullptr) {
                for (int ko = 0; ko < MONSCAPE_EEPROM_TOTAL_SIZE; ko += MONSCAPE_EEPROM_SLOT_SIZE) {
                    if (_storage->isSlotEmpty(ko)) {
                        sendTrame("msc", _storage->parseSlot(ko));
                    }
                }
            }
            break;
        case ACTION_LOAD_EEPROM:
            if (_storage != nullptr) {
                _storage->format();
            }
            break;
        case ACTION_LOG_TRAME:
            logTrame();
            break;
        case ACTION_SPECIAL:
        case ACTION_NONE:
        default:
            break;
    }

    return action;
}

bool PuzzleEngine::sendTrame(const MString& to, const MString& command) {
    MString payload = buildSendTrameJson(to, command);
    if (_transport != nullptr) {
        return _transport->send(payload);
    }
    return false;
}

bool PuzzleEngine::logTrame() {
    MString payload = buildLogTrameJson();
    if (_transport != nullptr) {
        return _transport->send(payload);
    }
    return false;
}

bool PuzzleEngine::initTrame() {
    if (_transport != nullptr) {
        _transport->clear();
    }
    MString payload = buildInitTrameJson();
    if (_transport != nullptr) {
        return _transport->send(payload);
    }
    return false;
}

bool PuzzleEngine::listenServ() {
    if (_transport == nullptr) return false;
    if (_transport->available()) {
        MString incoming = _transport->readStringUntil('\n');
        if (incoming.length() > 0) {
            processIncomingJson(incoming);
            return true;
        }
    }
    return false;
}
