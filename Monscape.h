#ifndef Monscape_h
#define Monscape_h

#include "src/domain/GameState.h"
#include "src/domain/PuzzleEngine.h"
#include "src/ports/ITransportPort.h"
#include "src/ports/IStoragePort.h"
#include "src/ports/IActuatorPort.h"
#include "src/ports/IResetPort.h"

#include "src/adapters/ArduinoRS485Transport.h"
#include "src/adapters/ArduinoEepromStorage.h"
#include "src/adapters/ArduinoGpioRelay.h"
#include "src/adapters/ArduinoReset.h"

#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
#include <ArduinoJson.h>
#endif

class Monscape {
  protected:
    String _Nom_sys, _Adresse, _Ver_G, _State_Game, _Desc_Game, _Win_Code;
    String _Ver_ms = "0.2";
    int _Num_Relais;
    double _Temps;
    char _Pin_Relai[2];
    byte _Protocole;
    byte _RS485Pin;
    Trame _Trame;

#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
#if defined(ESP32)
    StaticJsonDocument<512> doc;
#else
    StaticJsonDocument<128> doc;
#endif
#endif

    // Hexagonal Engine & Default Adapters
    PuzzleEngine _engine;
    ArduinoGpioRelay _defaultRelay;
    ArduinoRS485Transport _defaultTransport;
    ArduinoEepromStorage _defaultStorage;
    ArduinoReset _defaultReset;

    bool RS485_Start(int Pinrx, int pintx, int baud);
    bool BasicCommand();
    bool Init();
    bool clear_Trame();
    char last_entry(char* header);
    int empty_slot(char* header);

  public:
    // Constructors (classic facade)
    Monscape(char relai, byte prtcl = MSCape_RS485);
    Monscape(char relai[2], byte prtcl = MSCape_RS485);

    // Constructor with injected ports (hexagonal architecture)
    Monscape(IActuatorPort* actuator, ITransportPort* transport,
             IStoragePort* storage = nullptr, IResetPort* reset = nullptr);

    // Access to internal hexagon engine
    PuzzleEngine& getEngine() { return _engine; }

    bool Desc_Game(String Nom_sys, String Adresse, String Ver_G, String Desc_Game, String Win_Code);
    bool Desc_Game(String Nom_sys, String Adresse, String Ver_G, String Ver_ms, String Desc_Game, String Win_Code);

    bool Communication(int Pinrx, int pintx, byte pinRS, int baud = 28800);

    bool Listenserv();

    bool Log_Trame();
    bool Init_Trame();

#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
#if defined(ESP32)
    void special_command(StaticJsonDocument<512> doc);
#else
    void special_command(StaticJsonDocument<128> doc);
    void special_command(StaticJsonDocument<256> doc);
#endif
#endif

    bool Puzzle();
    bool Fail();
    bool Win(bool bypass = false);
    bool Send_Trame(String To, String Commnand);

    void set_Trame_Input(char* value, int sizet);
    void set_Trame_Input(long value);
    void set_Trame_Input(int value);

    void set_Trame_Last_Input(char* value, int sizet);
    void set_Trame_Last_Input(int value);
    void set_Trame_Last_Input(long value);

    void set_Trame_Stat(char value);

    bool sendrs485(String buff);
    void write_eeprom(String data);
    String read_eeprom(unsigned int add);
    void read_Header_eeprom(char* value);
    void format_eeprom();
    String parse_Commande(String value);
    String code_Commande(int type, char* adru, char cmd, String data);
    String code_Commande(int type, char* adru, char cmd, char data);
    String code_Commande(int type, char* adru, char cmd, char* data);

    String parse_eeprom(int adr);
    String parse_eeprom64(unsigned int adr);
    String send_command_eeprom(int adr);
    int read_int_eeprom(int adr);
    unsigned char read_char_eeprom(unsigned int adr);
    bool is_emptyslot_eeprom(int adr);

    void set_Win(String Win_Code);
    void resetard();
};

#endif // Monscape_h
