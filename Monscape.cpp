#include "Monscape.h"

Monscape::Monscape(char relai, byte prtcl)
    : _defaultRelay(relai),
      _defaultTransport(0),
      _defaultStorage(),
      _defaultReset(),
      _engine(prtcl, &_defaultTransport, &_defaultStorage, &_defaultRelay, &_defaultReset)
{
  _Pin_Relai[0] = relai;
  _Pin_Relai[1] = 0;
  _Num_Relais = 1;
  _Protocole = prtcl;
  _RS485Pin = 0;
  _Trame.stat = MSC_IDLE;
  _Trame.input = "";
  _Trame.last_input = "";
  Init();
}

Monscape::Monscape(char relai[2], byte prtcl)
    : _defaultRelay(relai, 2),
      _defaultTransport(0),
      _defaultStorage(),
      _defaultReset(),
      _engine(prtcl, &_defaultTransport, &_defaultStorage, &_defaultRelay, &_defaultReset)
{
  _Pin_Relai[0] = relai[0];
  _Pin_Relai[1] = relai[1];
  _Num_Relais = 2;
  _Protocole = prtcl;
  _RS485Pin = 0;
  _Trame.stat = MSC_IDLE;
  _Trame.input = "";
  _Trame.last_input = "";
  Init();
}

Monscape::Monscape(IActuatorPort* actuator, ITransportPort* transport,
                   IStoragePort* storage, IResetPort* reset)
    : _defaultRelay(0),
      _defaultTransport(0),
      _defaultStorage(),
      _defaultReset(),
      _engine(MSCape_RS485, transport, storage, actuator, reset)
{
  _Num_Relais = 0;
  _Protocole = MSCape_RS485;
  _RS485Pin = 0;
  _Trame.stat = MSC_IDLE;
  _Trame.input = "";
  _Trame.last_input = "";
  if (actuator != nullptr) _engine.setActuatorPort(actuator);
  if (transport != nullptr) _engine.setTransportPort(transport);
  if (storage != nullptr) _engine.setStoragePort(storage);
  if (reset != nullptr) _engine.setResetPort(reset);
}

bool Monscape::Init()
{
  _defaultRelay.init();
  return true;
}

bool Monscape::Desc_Game(String Nom_sys, String Adresse, String Ver_G, String Desc_Game, String Win_Code)
{
  _Nom_sys = Nom_sys;
  _Adresse = Adresse;
  _Ver_G = Ver_G;
  _Desc_Game = Desc_Game;
  _Win_Code = Win_Code;
  _engine.descGame(Nom_sys, Adresse, Ver_G, Desc_Game, Win_Code);
  return true;
}

bool Monscape::Desc_Game(String Nom_sys, String Adresse, String Ver_G, String Ver_ms, String Desc_Game, String Win_Code)
{
  _Nom_sys = Nom_sys;
  _Adresse = Adresse;
  _Ver_G = Ver_G;
  _Ver_ms = Ver_ms;
  _Desc_Game = Desc_Game;
  _Win_Code = Win_Code;
  _engine.descGame(Nom_sys, Adresse, Ver_G, Desc_Game, Win_Code);
  return true;
}

void Monscape::set_Win(String Win_Code)
{
  _Win_Code = Win_Code;
  _engine.setWin(Win_Code);
}

bool Monscape::Communication(int Pinrx, int pintx, byte pinRS, int baud)
{
  _RS485Pin = pinRS;
  _defaultTransport.setRsPin(pinRS);
  _defaultTransport.begin(Pinrx, pintx, baud);

  clear_Trame();
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
  delay(1000);
#endif
  Init_Trame();
  return true;
}

bool Monscape::RS485_Start(int Pinrx, int pintx, int baud)
{
  return _defaultTransport.begin(Pinrx, pintx, baud);
}

bool Monscape::Log_Trame()
{
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
  String buffer;
  doc.clear();
  doc["Nom"] = _Nom_sys;
  doc["Adr"] = _Adresse;
  doc["stat"] = (int) _Trame.stat;
  serializeJson(doc, buffer);

  switch (_Protocole) {
    case MSCape_RS485:
      sendrs485(buffer);
      break;
    default:
      serializeJson(doc, Serial);
      Serial.println("");
      break;
  }
#else
  _engine.logTrame();
#endif
  return true;
}

bool Monscape::Init_Trame()
{
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
  String buffer;
  doc.clear();
  doc["Nom"] = _Nom_sys;
  doc["Adr"] = _Adresse;
  doc["V_G"] = _Ver_G;
  doc["V_mon"] = _Ver_ms;
  doc["Desc"] = _Desc_Game;
  doc["Win"] = _Win_Code;
  serializeJson(doc, buffer);

  switch (_Protocole) {
    case MSCape_RS485:
      sendrs485(buffer);
      break;
    default:
      serializeJson(doc, Serial);
      Serial.println("");
      break;
  }
#else
  _engine.initTrame();
#endif
  return true;
}

bool Monscape::Send_Trame(String To, String Commnand)
{
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
  String buffer;
  doc.clear();
  doc["Adr"] = _Adresse;
  doc["A"] = To;
  doc["C"] = "S";
  doc["exec"] = Commnand;
  serializeJson(doc, buffer);

  switch (_Protocole) {
    case MSCape_RS485:
      sendrs485(buffer);
      break;
    default:
      serializeJson(doc, Serial);
      Serial.println("");
      break;
  }
#else
  _engine.sendTrame(To, Commnand);
#endif
  return true;
}

bool Monscape::clear_Trame()
{
  _defaultTransport.clear();
  return true;
}

bool Monscape::sendrs485(String buffer)
{
  if (_engine.getTransportPort() != nullptr) {
    return _engine.getTransportPort()->send(buffer);
  }
  return _defaultTransport.send(buffer);
}

void Monscape::set_Trame_Last_Input(char* value, int sizet)
{
  String str(value);
  _Trame.last_input = str.substring(0, sizet);
  _engine.setTrameLastInput(_Trame.last_input);
}

void Monscape::set_Trame_Last_Input(int value)
{
  String str(value);
  _Trame.last_input = str;
  _engine.setTrameLastInput(_Trame.last_input);
}

void Monscape::set_Trame_Last_Input(long value)
{
  String str(value);
  _Trame.last_input = str;
  _engine.setTrameLastInput(_Trame.last_input);
}

void Monscape::set_Trame_Stat(char value)
{
  _Trame.stat = value;
  _engine.setTrameStat(value);
}

void Monscape::set_Trame_Input(char* value, int sizet)
{
  String str(value);
  _Trame.input = str.substring(0, sizet);
  _engine.setTrameInput(_Trame.input);
}

void Monscape::set_Trame_Input(long value)
{
  String str(value);
  _Trame.input = str;
  _engine.setTrameInput(_Trame.input);
}

void Monscape::set_Trame_Input(int value)
{
  String str(value);
  _Trame.input = str;
  _engine.setTrameInput(_Trame.input);
}

bool Monscape::Listenserv()
{
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
  if (_defaultTransport.available()) {
    String inData = _defaultTransport.readStringUntil('\n');
    if (inData.length() > 0) {
      deserializeJson(doc, inData);
      BasicCommand();
    }
    return true;
  }
  return false;
#else
  return _engine.listenServ();
#endif
}

bool Monscape::BasicCommand()
{
#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
  String targetAdresse = doc["A"];
  if (targetAdresse == _Adresse) {
    const char* Comm = doc["C"];
    if (Comm != nullptr) {
      switch (Comm[0]) {
        case 'R':
          Serial.println("Reset");
          delay(100);
          resetard();
          break;
        case 'B':
          Win(true);
          break;
        case 'X':
          for (int ko = 0; ko < 1024; ko += 64) {
            if (is_emptyslot_eeprom(ko)) {
              Send_Trame("msc", parse_eeprom(ko));
            }
          }
          break;
        case 'E':
          format_eeprom();
          for (int ko = 0; ko < MAXDATA; ko++) {
            write_eeprom(doc["D"][ko]);
          }
          break;
        case 'S':
          special_command(doc);
          break;
        case 'I':
          Log_Trame();
          break;
      }
    }
    doc.clear();
  }
#endif
  return true;
}

bool Monscape::Win(bool bypass)
{
  if (_engine.getActuatorPort() != nullptr) {
    _engine.getActuatorPort()->activate(0);
  } else {
    _defaultRelay.activate(0);
  }
#if (defined(ARDUINO) && ARDUINO >= 100) && !defined(ESP32)
  for (int ko = 0; ko < 1024; ko += 64) {
    if (is_emptyslot_eeprom(ko)) {
      Send_Trame("msc", send_command_eeprom(ko));
    }
  }
#endif
  if (bypass) {
    set_Trame_Stat(MSC_BYPASS);
  } else {
    set_Trame_Stat(MSC_WIN);
  }
  Log_Trame();
  return true;
}

bool Monscape::Fail()
{
  if (_engine.getActuatorPort() != nullptr) {
    _engine.getActuatorPort()->deactivate(0);
  } else {
    _defaultRelay.deactivate(0);
  }
  set_Trame_Stat(MSC_BUZY);
  Log_Trame();
  return true;
}

void Monscape::resetard()
{
  _defaultReset.reset();
}

void Monscape::write_eeprom(String data)
{
  _defaultStorage.writeData(data);
}

char Monscape::last_entry(char* header)
{
  char j = 0;
  for (int i = 0; i < MAXDATA; ++i) {
    if ((unsigned char) header[i] < 17) {
      if (j < header[i]) j = header[i];
    }
  }
  return j;
}

int Monscape::empty_slot(char* header)
{
  int i = 0;
  int j = 0;
  while (header[i] != 'e' && i < MAXDATA) {
    i++;
    j += 64;
  }
  return j;
}

void Monscape::read_Header_eeprom(char* value)
{
  _defaultStorage.readHeader(value);
}

void Monscape::format_eeprom()
{
  _defaultStorage.format();
}

String Monscape::read_eeprom(unsigned int add)
{
  return _defaultStorage.readString(add);
}

unsigned char Monscape::read_char_eeprom(unsigned int add)
{
  return _defaultStorage.readChar(add);
}

int Monscape::read_int_eeprom(int adr)
{
  return _defaultStorage.readInt(adr);
}

bool Monscape::is_emptyslot_eeprom(int adr)
{
  return _defaultStorage.isSlotEmpty(adr);
}

String Monscape::parse_eeprom(int adr)
{
  return _defaultStorage.parseSlot(adr);
}

String Monscape::send_command_eeprom(int adr)
{
  return _defaultStorage.sendCommandFromSlot(adr);
}

String Monscape::parse_eeprom64(unsigned int adr)
{
  String g = "";
  for (unsigned int i = adr; i < adr + 20; i++) {
    g = g + String((int) _defaultStorage.read(i));
  }
  return g;
}

String Monscape::code_Commande(int type, char* adru, char cmd, char data)
{
  return _engine.codeCommande(type, adru, cmd, data);
}

String Monscape::code_Commande(int type, char* adru, char cmd, String data)
{
  return _engine.codeCommande(type, adru, cmd, data);
}

String Monscape::code_Commande(int type, char* adru, char cmd, char* data)
{
  return _engine.codeCommande(type, adru, cmd, data);
}

String Monscape::parse_Commande(String value)
{
  return _engine.parseCommande(value);
}

#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
#if !defined(ESP32)
__attribute__((weak)) void Monscape::special_command(StaticJsonDocument<128> doc) { (void)doc; }
__attribute__((weak)) void Monscape::special_command(StaticJsonDocument<256> doc) { (void)doc; }
#else
__attribute__((weak)) void Monscape::special_command(StaticJsonDocument<512> doc) { (void)doc; }
#endif
#endif

__attribute__((weak)) bool Monscape::Puzzle() { return true; }
