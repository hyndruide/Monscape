#include <cassert>
#include <iostream>
#include "../src/domain/GameState.h"
#include "../src/domain/PuzzleEngine.h"
#include "../src/adapters/MockTransport.h"
#include "../src/adapters/MockStorage.h"
#include "../src/adapters/MockActuator.h"
#include "../src/adapters/MockReset.h"

void test_metadata_and_state() {
    MockTransport transport;
    MockStorage storage;
    MockActuator actuator(1);
    MockReset reset;

    PuzzleEngine engine(MSCape_RS485, &transport, &storage, &actuator, &reset);
    engine.descGame("PG1", "PG1", "1.0", "Knob puzzle", "4321");

    assert(engine.getNomSys() == "PG1");
    assert(engine.getAdresse() == "PG1");
    assert(engine.getVerG() == "1.0");
    assert(engine.getWinCode() == "4321");

    assert(engine.checkWin("4321") == true);
    assert(engine.checkWin("0000") == false);

    engine.setTrameStat(MSC_START);
    engine.setTrameInput("12");
    engine.setTrameLastInput("14");

    Trame t = engine.getTrame();
    assert(t.stat == MSC_START);
    assert(t.input == "12");
    assert(t.last_input == "14");

    std::cout << "[PASS] test_metadata_and_state" << std::endl;
}

void test_wire_protocol() {
    MockTransport transport;
    PuzzleEngine engine(MSCape_RS485, &transport);

    // Test codeCommande for VARIABLE (type=1)
    MString coded = engine.codeCommande(1, "PG1", 'W', (char) 5);
    assert(coded.length() > 0);

    // Test parseCommande with VARIABLE type
    // Format: value[0]=Type, value[1..3]=To, value[4]=Cmd, value[5]='1' (VARIABLE), value[6]=len, value[7]=data
    MString wireMsg = "1PG1W1\x01\x05";
    MString parsed = engine.parseCommande(wireMsg);
    assert(parsed.find("\"Type\":1") != std::string::npos);
    assert(parsed.find("\"To\":\"PG1\"") != std::string::npos);
    assert(parsed.find("\"Cmd\":\"W\"") != std::string::npos);
    assert(parsed.find("\"Data\":5") != std::string::npos);

    std::cout << "[PASS] test_wire_protocol" << std::endl;
}

void test_json_payloads_and_transport() {
    MockTransport transport;
    MockActuator actuator(1);
    PuzzleEngine engine(MSCape_RS485, &transport, nullptr, &actuator);
    engine.descGame("PG1", "PG1", "1.0", "Keypad", "1234");

    engine.initTrame();
    assert(transport.sentMessages.size() == 1);
    MString initMsg = transport.getLastSent();
    assert(initMsg.find("\"Nom\":\"PG1\"") != std::string::npos);
    assert(initMsg.find("\"Win\":\"1234\"") != std::string::npos);

    engine.sendTrame("msc", "RESET_ALL");
    assert(transport.sentMessages.size() == 2);
    MString sendMsg = transport.getLastSent();
    assert(sendMsg.find("\"exec\":\"RESET_ALL\"") != std::string::npos);

    std::cout << "[PASS] test_json_payloads_and_transport" << std::endl;
}

void test_win_and_bypass_triggers_actuator() {
    MockTransport transport;
    MockActuator actuator(1);
    PuzzleEngine engine(MSCape_RS485, &transport, nullptr, &actuator);
    engine.descGame("PG1", "PG1", "1.0", "Test", "1111");

    assert(actuator.isActive(0) == false);

    // Test normal win
    engine.win(false);
    assert(engine.getTrameStat() == MSC_WIN);
    assert(actuator.isActive(0) == true);

    // Reset actuator
    actuator.deactivate(0);
    assert(actuator.isActive(0) == false);

    // Test bypass
    engine.win(true);
    assert(engine.getTrameStat() == MSC_BYPASS);
    assert(actuator.isActive(0) == true);

    std::cout << "[PASS] test_win_and_bypass_triggers_actuator" << std::endl;
}

void test_incoming_command_handling() {
    MockTransport transport;
    MockActuator actuator(1);
    MockReset reset;
    PuzzleEngine engine(MSCape_RS485, &transport, nullptr, &actuator, &reset);
    engine.descGame("PG1", "PG1", "1.0", "Test", "1111");

    // Command addressed to someone else should be ignored
    transport.injectIncoming("{\"A\":\"OTHER\",\"C\":\"B\"}");
    bool processed = engine.listenServ();
    assert(processed == true);
    assert(actuator.isActive(0) == false);

    // Bypass command addressed to PG1
    transport.injectIncoming("{\"A\":\"PG1\",\"C\":\"B\"}");
    processed = engine.listenServ();
    assert(processed == true);
    assert(engine.getTrameStat() == MSC_BYPASS);
    assert(actuator.isActive(0) == true);

    // Reset command addressed to PG1
    assert(reset.resetCalled == false);
    transport.injectIncoming("{\"A\":\"PG1\",\"C\":\"R\"}");
    processed = engine.listenServ();
    assert(processed == true);
    assert(reset.resetCalled == true);

    std::cout << "[PASS] test_incoming_command_handling" << std::endl;
}

void test_storage_mock() {
    MockStorage storage;
    storage.format();
    assert(storage.isSlotEmpty(0) == false); // format sets 17, which is empty slot flag

    storage.write(0, 5);
    assert(storage.isSlotEmpty(0) == true); // < 17 means occupied/valid
    assert(storage.read(0) == 5);

    std::cout << "[PASS] test_storage_mock" << std::endl;
}

int main() {
    std::cout << "--- Running Monscape C++ Hexagonal Unit Tests ---" << std::endl;
    test_metadata_and_state();
    test_wire_protocol();
    test_json_payloads_and_transport();
    test_win_and_bypass_triggers_actuator();
    test_incoming_command_handling();
    test_storage_mock();
    std::cout << "All C++ Hexagonal tests passed successfully!" << std::endl;
    return 0;
}
