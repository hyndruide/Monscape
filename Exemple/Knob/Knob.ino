//#include <Encoder.h>
#include <Monscape.h> 
//for arduino with myserial software
#define RX_PIN A4
#define TX_PIN A5

//With ESP32 it use Serial2 GPIO RX 16 TX 17



// We create an object for the puzzle. 
/*First arg is for pin use for the relay 
 * the second is the choice of communication for now its RS485 the other come soon MSCape_RS485
 * 
 */
Monscape Puzz(12, MSCape_RS485);


/* This is where you can Create some special function for the node.js server 
 *  
 *  
 */ 
void Monscape::special_command(StaticJsonDocument<256> doc) {
  
  }

// here is where the codefor the puzzle is comming.
/*
 * you can use Win function for initiate the relay.
 */
bool Monscape::Puzzle() {



}





void setup() {
  //Descritipion of the game for node.js server
  Puzz.Desc_Game("Knob et laser", "PG2", "V1.0", "V0.2", "Knob", "laser sur la lentille");
  //Communication between node and arduino initialisation
  //the third arg is for the latch of RS485 you can find mor information about this protocole on internet
  Puzz.Communication(RX_PIN, TX_PIN, 14);
  //Init trame send to the server
  Puzz.Init_Trame();
  //set puzzle to start useable later...
  Puzz.set_Trame_Stat(MSC_START);
  //first trame send to the server
  Puzz.Log_Trame();

}

void loop() {
  //this one listen the server.
  Puzz.Listenserv();
  //this one launch the puzzle.
  Puzz.Puzzle();

}
