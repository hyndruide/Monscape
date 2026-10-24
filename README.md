# Monscape

Monscape is a monitoring and supervision system for escape games using Arduino / ESP32 as puzzle controllers and a Raspberry Pi (or PC) as the game conductor.

The system communicates over RS485, Serial, or network using JSON payloads.

---

## 🏛️ Architecture Hexagonale (Ports & Adaptateurs)

Monscape a été refactorisé selon les principes de l'**Architecture Hexagonale**, découplant la logique métier pure du matériel physique à la fois côté embarqué (C++) et côté serveur (Node.js).

```
                      ┌────────────────────────────────────────────────────────┐
                      │                 HEXAGONE (Cœur Métier)                 │
                      │                                                        │
    Driving Ports ──> │  [PuzzleEngine / PuzzleRegistry]                       │ ──> Driven Ports
    (Inbound Commands,│  • États : IDLE, START, BUZY, WIN, BYPASS              │     (Abstractions)
     Timer / Ticks)   │  • Logique de victoire, validation de codes, trames   │
                      └────────────────────────────────────────────────────────┘
                                   │                               │
                    ┌──────────────┴──────────────┐ ┌──────────────┴──────────────┐
                    ▼                             ▼ ▼                             ▼
              [Adaptateurs Entrants]                        [Adaptateurs Sortants]
              • SocketIoAdapter                             • ITransportPort :
              • ExpressHttpAdapter                            - ArduinoRS485Adapter
              • Serial Inbound Handler                        - MockTransport (Tests PC)
                                                            • IStoragePort :
                                                              - ArduinoEepromStorage
                                                              - MockStorage (In-Memory)
                                                            • IActuatorPort :
                                                              - ArduinoGpioRelay
                                                              - MockActuator
```

### Avantages majeurs
1. **Testabilité totale sans matériel** : Possibilité d'exécuter l'ensemble de la logique de jeu sur PC (Linux, macOS, Windows) grâce aux adaptateurs Mocks.
2. **Rétrocompatibilité Arduino** : L'interface de façade `Monscape` reste 100% compatible avec les croquis `.ino` existants (`Knob.ino`, `Template.ino`, `messi_one.ino`).
3. **Extensibilité** : Nouveaux modes de communication (WiFi, MQTT, Ethernet) ou de persistance ajoutables par simple implémentation d'un port.

---

## 🚀 Démarrage Rapide

### 1. Serveur Node.js (Raspberry Pi ou PC de Dev)

#### Installation
```bash
cd monscape_nodejs_serv
npm install
```

#### Exécution en mode Développement / Simulation (PC)
Si aucun port série `/dev/ttyS0` physique n'est détecté, le serveur bascule automatiquement en mode **Mock Serial**, ce qui permet de tester et développer l'interface web sans matériel :
```bash
npm start
# Le serveur est accessible sur http://localhost:8080
```

#### Exécution en Production sur Raspberry Pi
```bash
SERIAL_DEVICE=/dev/ttyS0 npm start
```

#### Variables d'environnement configurables
- `PORT` : Port HTTP du serveur (défaut : `8080`).
- `SERIAL_DEVICE` : Chemin du périphérique série (défaut : `/dev/ttyS0`).
- `BAUD_RATE` : Vitesse de communication RS485 (défaut : `74880`).
- `GPIO_RE` : Broche GPIO pour RS485 RE (défaut : `17`).
- `GPIO_DE` : Broche GPIO pour RS485 DE (défaut : `27`).
- `USE_MOCK_SERIAL` : Forcer le mock (`true` / `false`).

#### Lancer les tests unitaires Node.js
```bash
cd monscape_nodejs_serv
npm test
```

---

### 2. Bibliothèque C++ (Arduino & ESP32)

#### Utilisation classique (Façade rétrocompatible)
Installez le dossier de la bibliothèque dans votre répertoire Arduino `libraries/`.

```cpp
#include <Monscape.h>

#define RX_PIN A4
#define TX_PIN A5
#define RS485_RW 14

Monscape Puzz(12, MSCape_RS485);

void setup() {
  Puzz.Desc_Game("MonPuzzle", "PG1", "V1.0", "FormatShow", "1234");
  Puzz.Communication(RX_PIN, TX_PIN, RS485_RW);
  Puzz.set_Trame_Stat(MSC_START);
  Puzz.Log_Trame();
}

void loop() {
  Puzz.Listenserv();
  Puzz.Puzzle();
}
```

#### Utilisation avancée (Injection de dépendances / Test unitaire C++)
```cpp
#include "Monscape.h"
#include "src/adapters/MockTransport.h"
#include "src/adapters/MockStorage.h"
#include "src/adapters/MockActuator.h"

MockActuator mockRelay(1);
MockTransport mockTransport;
MockStorage mockStorage;

Monscape puzz(&mockRelay, &mockTransport, &mockStorage);
puzz.Desc_Game("PuzzleTest", "PG1", "1.0", "Desc", "4321");
puzz.Win(); // Active mockRelay et log la trame via mockTransport
```

#### Lancer les tests unitaires C++ natifs (sans Arduino physique)
```bash
g++ -std=c++14 -I. \
  src/domain/PuzzleEngine.cpp \
  src/adapters/BaseEepromStorage.cpp \
  test/test_hexagonal_cpp.cpp -o test_hexagonal && ./test_hexagonal
```

---

## 📁 Structure du Projet

```
Monscape/
├── Monscape.h                  # Façade C++ rétrocompatible
├── Monscape.cpp                # Implémentation de la façade C++
├── src/
│   ├── domain/                 # Cœur Métier C++ (Inside the Hexagon)
│   │   ├── GameState.h         # États, trames et types multiplateformes
│   │   ├── PuzzleEngine.h      # Machine à états et règles du jeu
│   │   └── PuzzleEngine.cpp
│   ├── ports/                  # Driven Ports C++ (Interfaces abstraites)
│   │   ├── ITransportPort.h    # Abstraction transport série/RS485/WiFi
│   │   ├── IStoragePort.h      # Abstraction EEPROM / persistance
│   │   ├── IActuatorPort.h     # Abstraction relais / actionneurs
│   │   └── IResetPort.h        # Abstraction reset matériel
│   └── adapters/               # Adaptateurs C++ (Outside the Hexagon)
│       ├── ArduinoRS485Transport.h/.cpp
│       ├── ArduinoEepromStorage.h/.cpp
│       ├── ArduinoGpioRelay.h/.cpp
│       ├── ArduinoReset.h
│       ├── BaseEepromStorage.h/.cpp
│       ├── MockTransport.h
│       ├── MockStorage.h
│       └── MockActuator.h
├── test/                       # Tests unitaires natifs C++
│   ├── test_hexagonal_cpp.cpp
│   └── test_facade_mock.cpp
├── monscape_nodejs_serv/       # Serveur Node.js (Supervision Game Master)
│   ├── package.json
│   ├── server.js               # Point d'entrée serveur
│   ├── src/
│   │   ├── config/             # Configuration et détection d'environnement
│   │   ├── domain/             # Entités et services métier JS
│   │   │   ├── Puzzle.js       # Entité Puzzle
│   │   │   ├── PuzzleRegistry.js # Registre des puzzles de la salle
│   │   │   └── CommandBuilder.js # Construction et parsing des trames
│   │   ├── ports/              # Ports abstraits JS
│   │   │   ├── ISerialTransportPort.js
│   │   │   └── IClientBroadcasterPort.js
│   │   ├── adapters/           # Adaptateurs Inbound & Outbound JS
│   │   │   ├── inbound/
│   │   │   │   ├── ExpressHttpAdapter.js
│   │   │   │   └── SocketIoAdapter.js
│   │   │   └── outbound/
│   │   │       ├── RS485HardwareSerialAdapter.js
│   │   │       └── MockSerialAdapter.js
│   │   └── app.js              # Racine de composition (Dependency Injection)
│   ├── test/                   # Tests automatisés Node.js
│   │   ├── domain.test.js
│   │   └── app.test.js
│   ├── views/                  # Vues EJS
│   └── public/                 # Assets statiques CSS / JS
└── Exemple/                    # Exemples de croquis Arduino existants
```

## Built With

* [Express](https://expressjs.com/) — Web framework for Node.js
* [Socket.io](https://socket.io/) — Real-time event communication
* [ArduinoJSON](https://arduinojson.org/) — JSON Arduino library
* [OnOff](https://www.npmjs.com/package/onoff) — GPIO access on Linux/Raspberry Pi

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
