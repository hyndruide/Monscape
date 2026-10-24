#include <cassert>
#include <iostream>
#include "../Monscape.h"
#include "../src/adapters/MockTransport.h"
#include "../src/adapters/MockStorage.h"
#include "../src/adapters/MockActuator.h"
#include "../src/adapters/MockReset.h"

// Implement dummy weak overrides as user sketch would
bool Monscape::Puzzle() {
    set_Trame_Input(12);
    set_Trame_Last_Input(14);
    Log_Trame();
    return true;
}

int main() {
    std::cout << "Testing Monscape Facade with injected mock ports..." << std::endl;
    MockActuator mockRelay(1);
    MockTransport mockTransport;
    MockStorage mockStorage;
    MockReset mockReset;

    Monscape puzz(&mockRelay, &mockTransport, &mockStorage, &mockReset);

    puzz.Desc_Game("TestGame", "PG1", "V1.0", "FormatShow", "1234");
    puzz.set_Trame_Stat(MSC_START);
    puzz.Init_Trame();

    assert(mockTransport.sentMessages.size() == 1);

    // Call sketch puzzle function
    puzz.Puzzle();
    assert(mockTransport.sentMessages.size() == 2);

    // Trigger Win
    puzz.Win();
    assert(mockRelay.isActive(0) == true);

    std::cout << "Facade mock test passed successfully!" << std::endl;
    return 0;
}
