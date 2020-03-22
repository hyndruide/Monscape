#ifndef Monscape_h
#define Monscape_h

#include "Arduino.h"

#if !defined(ESP32)
#include <SoftwareSerial.h>
#endif
#include <ArduinoJson.h>
#include <EEPROM.h>

#define MSCape_RJ45 2
#define MSCape_WIFI 4
#define MSCape_RS485 6
#define MSCape_SERIAL 8
#define MSCape_I2C 9

#define MSC_IDLE 4
#define MSC_START 6
#define MSC_BUZY 9
#define MSC_BYPASS 11
#define MSC_WIN 12

#define SOCKETIO 1
#define ARDUINO 2
#define SOUNDSERVER 3
#define MONSCAPE 4

#define VARIABLE 1
#define STRING 2
#define ARRAY 3

#define MAXDATA 16

struct Trame {
  String last_input;
  char stat;
  String input;
};



class Monscape {
  private:
    String _Nom_sys,_Adresse, _Ver_G, _State_Game, _Desc_Game, _Win_Code;
    String _Ver_ms = "0.2";
    int _Num_Relais;
    double _Temps;
    char _Pin_Relai[2];
    byte _Protocole;
    byte _RS485Pin;
    #if !defined(ESP32)
        SoftwareSerial *mySerial;
    #endif
    #if defined(ESP32)  
    StaticJsonDocument<512> doc;
    #else
    StaticJsonDocument<128> doc;
    #endif
    Trame _Trame;
    bool RS485_Start(int Pinrx, int pintx, int baud) ;
    bool BasicCommand();
    bool Init();
    bool Init_Trame();
    bool clear_Trame();
    char last_entry(char* header);
    int empty_slot(char* header);
  public:

    Monscape(char relai, byte prtcl);
    Monscape(char relai[2], byte prtcl);

    bool Desc_Game(String Nom_sys, String _Adresse, String Ver_G, String Desc_Game,String Win_Code);

    bool Communication(int Pinrx, int pintx , byte pinRS,int  baud = 28800);

    bool Listenserv();

    bool Log_Trame();
    #if defined(ESP32)
    void special_command(StaticJsonDocument<512> doc);
    #else
    void special_command(StaticJsonDocument<128> doc);
    #endif
    bool Puzzle();
    bool Fail();
    bool Win(bool bypass = false );
    bool Send_Trame(String To,String Commnand );
    void set_Trame_Input(char* value, int sizet);
    void set_Trame_Input(long value);
    void set_Trame_Input(int value);

    void set_Trame_Last_Input(char* value,int sizet);
    void set_Trame_Last_Input(int value);
    void set_Trame_Last_Input(long value);

    void set_Trame_Stat(char value);
    
    bool sendrs485(String buff);
    void write_eeprom(String data);
    String read_eeprom(unsigned int add);
    void read_Header_eeprom(char* value);
    void format_eeprom();
    String parse_Commande(String value);
    String code_Commande(int type,char * adru,char cmd,String data);
    String code_Commande(int type,char * adru,char cmd,char data);
    String code_Commande(int type,char * adru,char cmd,char* data);

    String parse_eeprom(int adr);
    String parse_eeprom64(unsigned int adr);
    String send_command_eeprom(int adr);
    int read_int_eeprom(int adr);
    unsigned char read_char_eeprom(unsigned int adr);
    bool is_emptyslot_eeprom(int adr);

    void set_Win(String Win_Code);


    void resetard();


};

#endif
