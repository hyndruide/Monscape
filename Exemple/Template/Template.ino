
#include <Monscape.h>



#define RX_PIN A4
#define TX_PIN A5
#define RS485_RW 14


/*
 * Monscape Class :
 * First arguement : Pin for relay
 * Second : communication Way
 * MSCape_RJ45 
 * MSCape_WIFI 
 * MSCape_RS485 (for now only this work.)
 * MSCape_SERIAL 
 * MSCape_I2C 
 */
Monscape Puzz(12, MSCape_RS485);



/*
 * Special_command func inner Puzz : 
 * use to add some new command for to the puzzle when server send them.
 * 
 * 
 * Basic_command already in :
 *    Reset.
 *    Win shorcut (Bypass).
 *    Send Trame.
 */

void Monscape::special_command(StaticJsonDocument<256> doc) {
  
  }

/*
 * Puzzle func inner Puzz :
 * this is where you add your puzzle.
 */
 
bool Monscape::Puzzle() {
     /*
 * set_Trame_Input :
 * add value (12) in "input" in the next trame send 
 * set_Trame_Last_Input :
 * add value (14) in "Last_input" in the next trame send 
 * Log_Trame : 
 * Send the trame to the server in json format.
 */
     set_Trame_Input(12);
     set_Trame_Last_Input(14);
     Log_Trame();

}





void setup() {
  /*
   * Desc_game :
   * title : title of th puzzle
   * adresse : use like IP adresse for reconize who talk i use 3 character like "PG1"
   * Version game 
   * Version of library
   * format show : usefull later to reconise which template is use on server to show the puzzle on screen.
   * how to win : short explaination for the code or the way to win the puzzle.
   */  
  Puzz.Desc_Game("title", "adresse", "Version Game", "Version of library", "formatshow","how to win");
    /*
   * Communication :
   * 1st : RXpin for arduino but esp use Serial2 
   * 2nd : TXpin for arduino but esp use Serial2
   * 3rd : RS485 pin for read /write way.
   */ 
  Puzz.Communication(RX_PIN, TX_PIN, RS485_RW);
  /*
   * Set the puzzle to start useable Later
   */ 
  Puzz.set_Trame_Stat(MSC_START);
  /*
   * Send the first trame.
   */ 
  Puzz.Log_Trame();

}

void loop() {
  //listen nodejs server.
  Puzz.Listenserv();
  //Start puzzle game.
  Puzz.Puzzle();

}
