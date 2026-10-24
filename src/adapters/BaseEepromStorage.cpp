#include "BaseEepromStorage.h"

char BaseEepromStorage::lastEntry(const char* header) {
    char j = 0;
    for (int i = 0; i < MAXDATA; ++i) {
        if ((unsigned char) header[i] < 17) {
            if (j < header[i]) j = header[i];
        }
    }
    return j;
}

int BaseEepromStorage::emptySlot(const char* header) {
    int i = 0;
    int j = 0;
    while (header[i] != 'e' && i < MAXDATA) {
        i++;
        j += MONSCAPE_EEPROM_SLOT_SIZE;
    }
    return j;
}

void BaseEepromStorage::readHeader(char* value) {
    int j = 0;
    for (int i = 0; i < MONSCAPE_EEPROM_TOTAL_SIZE && j < MAXDATA; i += MONSCAPE_EEPROM_SLOT_SIZE) {
        uint8_t k = read(i);
        if (k >= 16) {
            value[j] = 'e';
        } else {
            value[j] = (char) k;
        }
        j++;
    }
}

void BaseEepromStorage::format() {
    for (int i = 0; i < MONSCAPE_EEPROM_TOTAL_SIZE; i += MONSCAPE_EEPROM_SLOT_SIZE) {
        uint8_t k = read(i);
        if (k < 17) {
            write(i, 17);
        }
    }
}

bool BaseEepromStorage::isSlotEmpty(unsigned int addr) {
    uint8_t k = read(addr);
    return (k < 17);
}

void BaseEepromStorage::writeData(const MString& data) {
    char header[MAXDATA];
    for (int j = 0; j < MAXDATA; j++) header[j] = '\0';

    readHeader(header);
    char last = lastEntry(header);
    int size = data.length();

    if (last == 0) {
        write(0, last + 1);
        for (int i = 1; i <= size; i++) {
            write(i, (uint8_t) data[i - 1]);
        }
    } else {
        int slot = emptySlot(header);
        write(slot, last + 1);
        slot++;
        for (int i = 0; i < size; i++) {
            write(slot + i, (uint8_t) data[i]);
        }
    }
}

MString BaseEepromStorage::readString(unsigned int addr) {
    char k = (char) read(addr);
    return MString(k);
}

unsigned char BaseEepromStorage::readChar(unsigned int addr) {
    return (unsigned char) read(addr);
}

int BaseEepromStorage::readInt(unsigned int addr) {
    unsigned char k = (unsigned char) read(addr);
    return (int) k - '0';
}

MString BaseEepromStorage::parseSlot(unsigned int adr) {
    MString parse_data = MString("{\"Ordre\":") + MString((int) read(adr)) +
                         MString(",\"Type\":") + MString(readInt(adr + 1)) +
                         MString(",\"To\":\"") + readString(adr + 2) + readString(adr + 3) + readString(adr + 4) +
                         MString("\",\"Cmd\":\"") + readString(adr + 5) + MString("\",");

    int type = readInt(adr + 6);
    switch (type) {
        case VARIABLE:
            parse_data = parse_data + MString("\"Data\":") + MString((int) readChar(adr + 8));
            break;
        case STRING: {
            parse_data = parse_data + MString("\"Data\":\"");
            int len = (int) readChar(adr + 7);
            for (int i = 0; i < len; i++) {
                parse_data = parse_data + readString(adr + 8 + i);
            }
            parse_data = parse_data + MString("\"");
            break;
        }
        case ARRAY: {
            parse_data = parse_data + MString("\"Data\":[");
            int len = (int) readChar(adr + 7);
            for (int i = 0; i < len; i++) {
                parse_data = parse_data + MString((int) readChar(adr + 8 + i));
                parse_data = parse_data + MString(",");
            }
            if (len > 0) {
                parse_data = parse_data.substring(0, parse_data.length() - 1);
            }
            parse_data = parse_data + MString("]");
            break;
        }
        default:
            break;
    }

    parse_data = parse_data + MString("}");
    return parse_data;
}

MString BaseEepromStorage::sendCommandFromSlot(unsigned int adr) {
    MString parse_data = MString("{\"Type\":") + MString(readInt(adr + 1)) +
                         MString(",\"To\":\"") + readString(adr + 2) + readString(adr + 3) + readString(adr + 4) +
                         MString("\",\"Cmd\":\"") + readString(adr + 5) + MString("\",");

    int type = readInt(adr + 6);
    switch (type) {
        case VARIABLE:
            parse_data = parse_data + MString("\"Data\":") + MString((int) readChar(adr + 8));
            break;
        case STRING: {
            parse_data = parse_data + MString("\"Data\":\"");
            int len = (int) readChar(adr + 7);
            for (int i = 0; i < len; i++) {
                parse_data = parse_data + readString(adr + 8 + i);
            }
            parse_data = parse_data + MString("\"");
            break;
        }
        case ARRAY: {
            parse_data = parse_data + MString("\"Data\":[");
            int len = (int) readChar(adr + 7);
            for (int i = 0; i < len; i++) {
                parse_data = parse_data + MString((int) readChar(adr + 8 + i));
                parse_data = parse_data + MString(",");
            }
            if (len > 0) {
                parse_data = parse_data.substring(0, parse_data.length() - 1);
            }
            parse_data = parse_data + MString("]");
            break;
        }
        default:
            break;
    }

    parse_data = parse_data + MString("}");
    return parse_data;
}
