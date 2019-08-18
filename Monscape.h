#ifndef Monscape_h
#define Monscape_h

#include "Arduino.h"

#if !defined(ESP32)
#include <SoftwareSerial.h>
#endif
#include <ArduinoJson.h>


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

struct Trame {
  String last_input;
  char stat;
  String input;
};


class Monscape {
  private:
    String _Nom_sys,_Adresse, _Ver_G, _Ver_ms, _State_Game, _Desc_Game, _Win_Code;
    String _Aide[4];
    int _Num_Relais;
    double _Temps;
    int _Pin_Relai[2];
    byte _Protocole;
    byte _RS485Pin;
    #if !defined(ESP32)
        SoftwareSerial *mySerial;
    #endif
    StaticJsonDocument<256> doc;
    Trame _Trame;
    bool RS485_Start(int Pinrx, int pintx, int baud) ;
    bool BasicCommand();
    bool Init();
    bool Init_Trame();

  public:

    Monscape(int relai, byte prtcl);
    Monscape(int relai[2], byte prtcl);

    bool Desc_Game(String Nom_sys, String _Adresse, String Ver_G, String Ver_ms, String Desc_Game,String Win_Code);

    bool Communication(int Pinrx, int pintx , byte pinRS);

    bool Listenserv();

    bool Log_Trame();
    void special_command(StaticJsonDocument<256> doc);
    bool Puzzle();
    bool Fail();
    bool Win(bool bypass );

    void set_Trame_Input(char* value, int sizet);
    void set_Trame_Input(long value);
    void set_Trame_Input(int value);

    void set_Trame_Last_Input(char* value,int sizet);
    void set_Trame_Last_Input(int value);
    void set_Trame_Last_Input(long value);

    void set_Trame_Stat(char value);
    
    void set_Win(String Win_Code);


    void resetard();


};

#endif
