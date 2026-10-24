#ifndef GAMESTATE_H
#define GAMESTATE_H

#if (defined(ARDUINO) && ARDUINO >= 100) || defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ESP32)
  #include <Arduino.h>
  typedef String MString;
#else
  #include <string>
  #include <cstdint>
  #include <cstring>
  #include <sstream>

  typedef uint8_t byte;

  // Lightweight wrapper providing Arduino String-like methods on standard C++ (PC/Tests)
  class MString : public std::string {
  public:
      MString() : std::string() {}
      MString(const char* s) : std::string(s ? s : "") {}
      MString(const std::string& s) : std::string(s) {}
      MString(int v) {
          std::ostringstream ss;
          ss << v;
          assign(ss.str());
      }
      MString(long v) {
          std::ostringstream ss;
          ss << v;
          assign(ss.str());
      }
      MString(unsigned int v) {
          std::ostringstream ss;
          ss << v;
          assign(ss.str());
      }
      MString(double v) {
          std::ostringstream ss;
          ss << v;
          assign(ss.str());
      }
      MString(char c) : std::string(1, c) {}

      MString substring(size_t from, size_t to) const {
          if (from >= length()) return MString("");
          size_t count = (to > from) ? (to - from) : 0;
          return MString(substr(from, count));
      }

      MString substring(size_t from) const {
          if (from >= length()) return MString("");
          return MString(substr(from));
      }
  };

  typedef MString String;
#endif

// Communication protocols
#define MSCape_RJ45 2
#define MSCape_WIFI 4
#define MSCape_RS485 6
#define MSCape_SERIAL 8
#define MSCape_I2C 9

// Game states
#define MSC_IDLE 4
#define MSC_START 6
#define MSC_BUZY 9
#define MSC_BYPASS 11
#define MSC_WIN 12

// Targets
#define SOCKETIO 1
#define ARDUINO 2
#define SOUNDSERVER 3
#define MONSCAPE 4

// Data types for EEPROM / serial wire encoding
#define VARIABLE 1
#define STRING 2
#define ARRAY 3

#define MAXDATA 16
#define MONSCAPE_EEPROM_SLOT_SIZE 64
#define MONSCAPE_EEPROM_MAX_SLOTS 16
#define MONSCAPE_EEPROM_TOTAL_SIZE 1024

// Trame data structure
struct Trame {
  MString last_input;
  char stat;
  MString input;
};

// Engine command actions
enum CommandAction {
  ACTION_NONE = 0,
  ACTION_RESET,
  ACTION_BYPASS,
  ACTION_SEND_EEPROM,
  ACTION_LOAD_EEPROM,
  ACTION_SPECIAL,
  ACTION_LOG_TRAME
};

#endif // GAMESTATE_H
