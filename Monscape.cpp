#include "Monscape.h"

void Monscape::write_eeprom(String data)
{
  Serial.println(data);
  char header[MAXDATA];
  int slot = 0;
  for(int j = 0;j< MAXDATA;j++) header[j] = {'\0'};
  read_Header_eeprom(header);
  char last = last_entry(header);
  Serial.println(last,DEC);
  if(last == 0){
    EEPROM.write(0,last+1);
    int size = data.length();
    int i;
    for(i=1;i<size+1;i++)
    {
      EEPROM.write(i,data[i-1]);
    }
  }
  else{
  slot = empty_slot(header);

    EEPROM.write(slot,last+1);
    int size = data.length();
    int i;
    slot++;
    for(int i=0;i<size;i++)
    {
      EEPROM.write(slot+i,data[i]);
    }
  }

}

char Monscape::last_entry(char* header){
  char j = 0;
  for(int i = 0; i < MAXDATA; ++i){
    if (header[i] < 17){
      if (j<header[i]) j = header[i];
    }
  }

  return j;
}



int Monscape::empty_slot(char* header){
  int i = 0; 
  int j = 0;

  while(header[i]!='e'){
    i++;
    j = j+ 64;
  }
  return j;
}

//  void Monscape::del_Command(char add,String data)
// {
//   int _size = data.length();
//   int i;
//   for(i=0;i<_size;i++)
//   {
//     EEPROM.write(add+i,data[i]);
//   }
//   EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
// }


void Monscape::read_Header_eeprom(char* value)
{
  char k;
  int i,j=0;
  
  for (int i = 0; i < 1023; i=i+64)
  {
    //Serial.println(i);
      k=EEPROM.read(i);
      if (k >= 16){
        value[j] = 'e';
        j++;
      }
      else{
        value[j] = k;
        j++;
      }
      
  }
  
}


void Monscape::format_eeprom()
{
  char k;
  int i;
  
  for (int i = 0; i < 1023; i=i+64)
  {
      k=EEPROM.read(i);
      if (k < 17){
        EEPROM.write(i,17);
      }
  }
  
}


String Monscape::read_eeprom(unsigned int adr)
{
  char k;
    k=EEPROM.read(adr);

  return String(k); 
}

unsigned char Monscape::read_char_eeprom(unsigned int adr)
{
  char k;
    k=EEPROM.read(adr);

  return k; 
}


int Monscape::read_int_eeprom(int adr)
{
  unsigned char k;
    k=EEPROM.read(adr);
    //Serial.print(k,DEC);
  return (int) k - '0'; 
}



String Monscape::code_Commande(int type,char * adru,char cmd,char data){
  String eepvle = String(type);
  eepvle = eepvle + String(adru);
  eepvle = eepvle + String(cmd);
  eepvle = eepvle + String("1");
  eepvle = eepvle + String((char) 1);
  eepvle = eepvle + String(data);
  return eepvle;
}

String Monscape::code_Commande(int type,char * adru,char cmd,String data){
  String eepvle = String(type);
  Serial.println(adru[1]);
  eepvle = eepvle + String(adru);
  eepvle = eepvle + String(cmd);
  eepvle = eepvle + String("2");
  eepvle = eepvle + (char) data.length();
  eepvle = eepvle + String(data);
return eepvle;
}

String Monscape::code_Commande(int type,char * adru,char cmd,char* data){
  String eepvle = String(type);
  eepvle = eepvle + String(adru);
  eepvle = eepvle + String(cmd);
  eepvle = eepvle + String("3");

  eepvle = eepvle + String((char) strlen(data));
  eepvle = eepvle + String(data);
return eepvle;
}


String Monscape::parse_Commande(String value){
  int i=0;
   String parse_data = String("{\"Type\":") + value[0] + String(",\"To\":\"") + value[1] + value[2] + value[3] + String("\",\"Cmd\":\"") + value[4] + String("\",");

   switch((int) value[5] - '0'){
    case VARIABLE :
      parse_data = parse_data + String("\"Data\":") + String((unsigned char) value[7]) ;
    break;
    case STRING :
      parse_data = parse_data + String("\"Data\":\"");
      for( i=0; i < value[6];i++ ){
         parse_data = parse_data +  value[7+i];
      } 
      parse_data = parse_data +  String("\"");

    break;
    case ARRAY :
      parse_data = parse_data + String("\"Data\":[");
      for( i=0; i < value[6] ;i++ ){
         parse_data = parse_data +  String((unsigned char) value[7+i]) ;
         parse_data = parse_data +  String(",");
      } 

      parse_data = parse_data.substring(0, parse_data.length() - 1);

      parse_data =  parse_data +  String("]");
    break;

   }
   parse_data =  parse_data +  String("}");
   return parse_data;

}

bool Monscape::is_emptyslot_eeprom(int adr){
    char k;

      k=EEPROM.read(adr);
      if (k < 17) return true;
      else return false;
}

String Monscape::parse_eeprom(int adr){
  int i=0;
   String parse_data =String("{\"Ordre\":") + String((int) EEPROM.read(adr)) +String(",\"Type\":") + String(read_int_eeprom(adr + 1)) + String(",\"To\":\"") + read_eeprom(adr + 2) + read_eeprom(adr + 3) + read_eeprom(adr + 4) + String("\",\"Cmd\":\"") + read_eeprom(adr + 5) + String("\",");

   switch(read_int_eeprom(adr+6)){
    case VARIABLE :
      parse_data = parse_data + String("\"Data\":") + String((int) read_char_eeprom(adr + 8));
    break;
    case STRING :
      parse_data = parse_data + String("\"Data\":\"");
      for( i=0; i < read_char_eeprom(adr + 7) ;i++ ){
         parse_data = parse_data +  read_eeprom(adr + 8 + i);
      } 
      parse_data = parse_data +  String("\"");

    break;
    case ARRAY :
      parse_data = parse_data + String("\"Data\":[");
      for( i=0; i < read_char_eeprom(adr + 7) ;i++ ){
         parse_data = parse_data +  String((int) read_char_eeprom(adr + 8 + i)) ;
         parse_data = parse_data +  String(",");
      } 

      parse_data = parse_data.substring(0, parse_data.length() - 1);

      parse_data =  parse_data +  String("]");
    break;

   }
   parse_data =  parse_data +  String("}");
   return parse_data;

}


String Monscape::send_command_eeprom(int adr){
  int i=0;
   String parse_data =String("{\"Type\":") + String(read_int_eeprom(adr + 1)) + String(",\"To\":\"") + read_eeprom(adr + 2) + read_eeprom(adr + 3) + read_eeprom(adr + 4) + String("\",\"Cmd\":\"") + read_eeprom(adr + 5) + String("\",");

   switch(read_int_eeprom(adr+6)){
    case VARIABLE :
      parse_data = parse_data + String("\"Data\":") + String((int) read_char_eeprom(adr + 8));
    break;
    case STRING :
      parse_data = parse_data + String("\"Data\":\"");
      for( i=0; i < read_char_eeprom(adr + 7) ;i++ ){
         parse_data = parse_data +  read_eeprom(adr + 8 + i);
      } 
      parse_data = parse_data +  String("\"");

    break;
    case ARRAY :
      parse_data = parse_data + String("\"Data\":[");
      for( i=0; i < read_char_eeprom(adr + 7) ;i++ ){
         parse_data = parse_data +  String((int) read_char_eeprom(adr + 8 + i)) ;
         parse_data = parse_data +  String(",");
      } 

      parse_data = parse_data.substring(0, parse_data.length() - 1);

      parse_data =  parse_data +  String("]");
    break;

   }
   parse_data =  parse_data +  String("}");
   return parse_data;

}


String Monscape::parse_eeprom64(unsigned int adr){
  int i=0;
  String g = "";
  for (int i = adr; i < adr+20; i++)
  {
      g = g + String(EEPROM.read(i));
  }

}


Monscape::Monscape(char relai, byte prtcl) {
  _Pin_Relai[0] = relai;
  _Num_Relais = 1;
  _Protocole = prtcl;
  Init();
};

Monscape::Monscape(char relai[2], byte prtcl) {
  _Pin_Relai[0] = relai[0];
  _Pin_Relai[1] = relai[1];
  _Num_Relais = 2;
  _Protocole = prtcl;
  Init();
};


bool Monscape::Init()
{

  for (int i = 0 ; i == _Num_Relais; i++) pinMode(_Pin_Relai[i], OUTPUT);

}

bool Monscape::Desc_Game(String Nom_sys, String Adresse, String Ver_G, String Desc_Game, String Win_Code)
{
  _Nom_sys = Nom_sys;
  _Ver_G = Ver_G;
  _Desc_Game = Desc_Game;
  _Win_Code = Win_Code;
  _Adresse = Adresse;
}



void Monscape::set_Win(String Win_Code){
  _Win_Code = Win_Code;
}



bool Monscape::Communication(int Pinrx, int pintx , byte pinRS,int  baud)
{

  _RS485Pin = pinRS;
  pinMode(_RS485Pin, OUTPUT);
  digitalWrite(_RS485Pin, LOW);
  switch (_Protocole) {

    case MSCape_RJ45:
    break;
    case MSCape_WIFI:
      // statements
    break;
    case MSCape_RS485:
#if defined(ESP32)
    Serial.begin(115200);
    Serial.println("Communication OK");
    RS485_Start(Pinrx,pintx,baud);
#else
    Serial.begin(115200);
    Serial.println("Communication OK");
    RS485_Start(Pinrx,pintx,baud);
      //mySerial->println("Communication RS485 OK");
    Serial.println("Done!");
#endif

    break;
    case MSCape_I2C:
      // statements
    break;
    default:
    Serial.begin(115200);
    Serial.println("Communication OK");
    break;
  }
  clear_Trame();
  delay(1000);
  Init_Trame();
}

bool Monscape::RS485_Start(int Pinrx, int pintx,int baud) {


#if defined(ESP32)
  Serial2.begin(baud, SERIAL_8N1, Pinrx, pintx);
#else
  mySerial = new SoftwareSerial(Pinrx, pintx);
  mySerial->begin(baud);
#endif
  Serial.println("Communication RS485");
}

bool Monscape::Log_Trame() {
  String buffer;
  doc.clear();
  doc["Nom"] = _Nom_sys;
  doc["Adr"] = _Adresse;
  doc["stat"] = _Trame.stat;

  //doc["last_modification"] = time();

  switch (_Protocole) {
    case MSCape_RJ45:
    break;
    case MSCape_WIFI:
      // statements
    break;


    case MSCape_RS485:
    serializeJson(doc, buffer);
    Serial.println("Send trame");
    digitalWrite(_RS485Pin, HIGH);
    //delay(10);
#if defined(ESP32)
    Serial2.println(buffer);
#else
    mySerial->println(buffer);
#endif
    serializeJson(doc, Serial);
    Serial.println("");
    //delay(40);
    digitalWrite(_RS485Pin, LOW);
    break;


    case MSCape_I2C:
      // statements
    break;
    default:
    serializeJson(doc, Serial);
    break;
  }


}

bool Monscape::Send_Trame(String To,String Commnand ){
  String buffer;
  doc.clear();
  doc["Adr"] = _Adresse;
  doc["A"] = To;
  doc["C"] = "S";
  doc["exec"] = Commnand;
  switch (_Protocole) {
    case MSCape_RJ45:
    break;
    case MSCape_WIFI:
      // statements
    break;
    case MSCape_RS485:

    digitalWrite(_RS485Pin, HIGH);
    serializeJson(doc, buffer);
    
#if defined(ESP32)
    delay(30);
    Serial2.println(buffer);
    delay(buffer.length()*2);
#else

    mySerial->println(buffer);
#endif
    
    serializeJson(doc, Serial);
    Serial.println(' ');
    digitalWrite(_RS485Pin, LOW);

    break;
    case MSCape_I2C:
      // statements
    break;
    default:
    serializeJson(doc, Serial);
    break;
  }

}

bool Monscape::clear_Trame() {


  switch (_Protocole) {
    case MSCape_RJ45:
    break;
    case MSCape_WIFI:
      // statements
    break;
    case MSCape_RS485:


    digitalWrite(_RS485Pin, HIGH);
    
#if defined(ESP32)
    delay(30);
    Serial2.println("");
    delay(100);
#else
    mySerial->println("");
#endif
    digitalWrite(_RS485Pin, LOW);

    break;
    case MSCape_I2C:
      // statements
    break;
    default:

    break;

  }


}


bool Monscape::Init_Trame() {
  String buffer;
  doc.clear();
  //_Nom_sys, _Ver_G, _Ver_ms, _Desc_Game;
  doc["Nom"] = _Nom_sys;
  doc["Adr"] = _Adresse;
  doc["V_G"] = _Ver_G;
  doc["V_mon"] = _Ver_ms ;
  doc["Desc"] = _Desc_Game;
  doc["Win"] = _Win_Code;

  serializeJson(doc, buffer);

  switch (_Protocole) {
    case MSCape_RJ45:
    break;
    case MSCape_WIFI:
      // statements
    break;

    case MSCape_RS485:

    digitalWrite(_RS485Pin, HIGH);
    
#if defined(ESP32)
    delay(30);
    Serial2.println(buffer);
    delay(buffer.length()*2);
#else
    delay(buffer.length()*2);
    mySerial->println(buffer);
    delay(buffer.length()*2);
#endif
    serializeJson(doc, Serial);
    Serial.println(' ');
    digitalWrite(_RS485Pin, LOW);

    break;

    case MSCape_I2C:
      // statements
    break;
    default:
    serializeJson(doc, Serial);
    break;
    doc.clear();
  }


}

//  String last_input;
//  char stat;
//  char input;

void Monscape::set_Trame_Last_Input(char* value, int sizet) {
  String str(value);
  _Trame.last_input = str.substring(0, sizet);
}

void Monscape::set_Trame_Last_Input(int value) {
  String str(value);
  _Trame.last_input = str;
}

void Monscape::set_Trame_Last_Input(long value) {
  String str(value);
  _Trame.last_input = str;
}

void Monscape::set_Trame_Stat(char value) {
  _Trame.stat = value;
}

void Monscape::set_Trame_Input(char* value, int sizet) {
  String str(value);
  _Trame.input = str.substring(0, sizet);
}

void Monscape::set_Trame_Input(long value) {
  String str(value);
  _Trame.input = str;
}

void Monscape::set_Trame_Input(int value) {
  String str(value);
  _Trame.input = str;
}

#if defined(ESP32)
bool Monscape::Listenserv() {
  String inData;
//Serial.println("ecoute");
  digitalWrite(_RS485Pin, LOW);
  if (Serial2.available() > 0)
  {
    Serial.println("Esp32 recept");
    inData = Serial2.readStringUntil('\n');
    Serial.println("B");
    Serial.println("data: " + inData);
    deserializeJson(doc, inData);
    BasicCommand();

    //else Serial.print("not for me");
    inData = "";

  }
}

#else

bool Monscape::Listenserv() {
  String inData;
  digitalWrite(_RS485Pin, LOW);
  if (mySerial->available() > 0)
  {
    //Serial.println("A");
    inData = mySerial->readStringUntil('\n');
    //Serial.println("B");
    //Serial.println("data: " + inData);
    deserializeJson(doc, inData);
    BasicCommand();
    inData = "";
  }
}

#endif



bool Monscape::BasicCommand(){
  String Adresse = doc["A"];
    //Serial.println(Adresse);
  int ko = 0;
  if (Adresse == _Adresse) {
      //Serial.println("commande passé");
    const char* Comm = doc["C"];
      //Serial.println(Comm);
    switch (Comm[0]) {
      case 'R':
      Serial.println("Reset");
      delay(100);
      resetard();
      break;
      case 'B':
      Win(true);
      break;
      case 'X': //send eeprom info
        for(ko = 0;ko <1024;ko = ko +64){
          if (is_emptyslot_eeprom(ko)) Send_Trame("msc",parse_eeprom(ko));
        }

      break;

      case 'E': //Load and eeprom 
      format_eeprom();
        for(ko = 0;ko < MAXDATA; ko++){
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
    doc.clear();
  }

}


bool Monscape::Win(bool bypass)
{
#if !defined(ESP32)
  for(int ko = 0;ko <1024;ko = ko +64){
    if (is_emptyslot_eeprom(ko)) Send_Trame("msc",send_command_eeprom(ko));

  }

#endif
  if (bypass == true)  set_Trame_Stat(MSC_BYPASS);
  else set_Trame_Stat(MSC_WIN);
  Log_Trame();
}



void Monscape::resetard() {

  #if defined(ESP32)
  ESP.restart();
#else
  asm volatile ("  jmp 0");  //declare reset function @ address 0
  #endif
}



