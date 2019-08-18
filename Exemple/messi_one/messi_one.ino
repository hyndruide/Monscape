#include <Monscape.h>
#include <Keypad.h>
#include <PololuLedStrip.h>
#include <EEPROM.h>


// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<7> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 5
rgb_color colors[LED_COUNT];

//#define MSCape_RJ45 2
//#define MSCape_WIFI 4
//#define MSCape_RS485 6
//#define MSCape_SERIAL 8
//#define MSCape_I2C 9

#define RX_PIN A4
#define TX_PIN A5

const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {8, 2, 3, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 9, 4}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//SoftwareSerial mySerial(RX_PIN, TX_PIN);

char pass[5] = {'?', '?', '?', '?', '\0'};
char gpass[5] = {'1', '2', '3', '4', '\0'};
int counter = 0;

Monscape Puzz(12, MSCape_RS485);


boolean array_cmp(char *a, char *b, int len_a, int len_b) {
  int n;

  // if their lengths are different, return false
  if (len_a != len_b) return false;

  // test each element to be the same. if not, return false
  for (n = 0; n < len_a; n++) if (a[n] != b[n]) return false;

  //ok, if we have not returned yet, they are equal :)
  return true;
}

bool Monscape::Puzzle() {

  char customKey = customKeypad.getKey();
  if (customKey) {
    tone(A0,440,500);

    pass[counter] =  customKey;
    set_Trame_Input(pass, sizeof(pass));
    Log_Trame();
    counter++;
    if (counter == 4) {
      tone(A0,320,500);
      counter = 0;
      set_Trame_Last_Input(pass, sizeof(pass));
      Log_Trame();
      Serial.println(pass);
      if (array_cmp(pass, gpass, sizeof(pass), sizeof(gpass)) == true) {
        tone(A0,880,500);
        Win();

      }
      pass[0] = '?';
      pass[1] = '?';
      pass[2] = '?';
      pass[3] = '?';
    }
  }

}


void Monscape::special_command(StaticJsonDocument<256> doc) {
  serializeJson(doc, Serial);
  const char* Comm = doc["C"];
  char R, V, B;
  char * pas;
  int num,t;
  switch (Comm[0]) {
    case 'W':
      pas = doc["D"];
      num = sizeof(doc["D"]);
      t = sizeof(gpass);
      if (t != num + 1) {
        return 0;
      }
      for (uint16_t i = 0; i < t - 1 ; i++)gpass[i]  = pas[i];
      for (uint16_t i = 0; i < t - 1 ; i++)EEPROM.write(i, gpass[i]);
      _Win_Code = String(gpass);
      Log_Trame();
      break;
    case 'L':
      Serial.println("Light");
      R = doc["D"][0];
      V = doc["D"][1];
      B = doc["D"][2];
      rgb_color color;
      color.red = R;
      color.green = V;
      color.blue = B;
      for (uint16_t i = 0; i < LED_COUNT; i++)
      {
        colors[i] = color;
      }

      // Write to the LED strip.
      ledStrip.write(colors, LED_COUNT);
      break;
  }

}





void setup() {
  for (uint16_t i = 0; i < 4 ; i++)gpass[i]  = EEPROM.read(i);;
  String str(gpass);
  Puzz.Desc_Game("Clavier", "PG1", "V1.0", "V0.01", "Keypad", str);
  Puzz.Communication(RX_PIN, TX_PIN, 10);
  Puzz.Init_Trame();
  Puzz.set_Trame_Stat(MSC_START);
  Puzz.Log_Trame();

}

void loop() {
  Puzz.Listenserv();
  Puzz.Puzzle();
}
