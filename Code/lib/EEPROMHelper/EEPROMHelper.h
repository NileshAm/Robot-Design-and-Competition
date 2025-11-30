#ifndef EEPROM_HELPER_H
#define EEPROM_HELPER_H

#include <Arduino.h>
#include <EEPROM.h>

class EEPROMHelper {
public:
    EEPROMHelper(int bufferSize) : _bufferSize(bufferSize) {
        if (_bufferSize > EEPROM.length()) {
            _bufferSize = EEPROM.length(); // cap to EEPROM size
        }
    }

    /* ------------ WRITE OVERLOADS ------------ */

    void write(int address, int value) {
        checkBounds(address, sizeof(int));
        EEPROM.put(address, value);
    }

    void write(int address, float value) {
        checkBounds(address, sizeof(float));
        EEPROM.put(address, value);
    }

    void write(int address, bool value) {
        checkBounds(address, sizeof(bool));
        EEPROM.put(address, value);
    }

    /* ------------ READ OVERLOADS ------------ */

    int readInt(int address) {
        checkBounds(address, sizeof(int));
        int value;
        EEPROM.get(address, value);
        return value;
    }

    float readFloat(int address) {
        checkBounds(address, sizeof(float));
        float value;
        EEPROM.get(address, value);
        return value;
    }

    bool readBool(int address) {
        checkBounds(address, sizeof(bool));
        bool value;
        EEPROM.get(address, value);
        return value;
    }

private:
    int _bufferSize;

    void checkBounds(int address, int dataSize) {
        if (address < 0 || (address + dataSize) > _bufferSize) {
            Serial.println("EEPROM ERROR: Address out of bounds!");
            while (1);
        }
    }
};

#endif
