#include <Encoder.h>
#include <Monscape.h>
#define RX_PIN A4 //RO BY
#define TX_PIN A5//DI

Monscape Puzz(12, MSCape_RS485);

unsigned long oldMillis = 0, deltamillis = 500;
int vitesse = 0, counter =  0, win_counter = 10;
int clickoldstat = 0;
bool flag_counter = true;

void Monscape::special_command(StaticJsonDocument<256> doc) {

}


bool Monscape::Puzzle() {

  unsigned long currentMillis = millis();

  int clickstat = digitalRead(A1);
  if (clickoldstat!=clickstat){
    vitesse += 1;
    clickoldstat = clickstat;
  }


  
  
 if( currentMillis - oldMillis >=  deltamillis){
 oldMillis = currentMillis ;
  if (vitesse != 0) {
     set_Trame_Input(vitesse);
     set_Trame_Last_Input(counter);
     Log_Trame();
     if(vitesse >= 50 && vitesse <= 80){
      counter +=1;
      Serial.println(counter);
     }
     else counter = 0;
     flag_counter = true;
     
  }
  else {
    if (flag_counter){
     set_Trame_Input(vitesse);
     Log_Trame();
     flag_counter = false;
     counter = 0;
    }
  }
  if (counter == win_counter) Win();
    vitesse = 0;
  }

  




}





void setup() {
  pinMode(A1, INPUT_PULLUP);
  Puzz.Desc_Game("Engrenage", "PG3", "V1.0", "V0.01", "Speed", "mettre l'engrenage tourner a la bonne vitesse");
  Puzz.Communication(RX_PIN, TX_PIN, 11);
  Puzz.Init_Trame();
  Puzz.set_Trame_Stat(MSC_START);
  Puzz.Log_Trame();

}

void loop() {
  Puzz.Listenserv();
  Puzz.Puzzle();

}
