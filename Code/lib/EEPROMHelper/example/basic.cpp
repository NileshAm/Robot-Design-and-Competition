#include <Arduino.h>
#include "EEPROMHelper.h"

EEPROMHelper eeprom(1024);  // Mega has 4KB EEPROM; use as needed

void setup() {
    Serial.begin(115200);

    // Write values
    eeprom.write(0, 123);          // int
    eeprom.write(10, 3.14f);       // float
    eeprom.write(20, true);        // bool

    // Read values
    int i = eeprom.readInt(0);
    float f = eeprom.readFloat(10);
    bool b = eeprom.readBool(20);

    Serial.println(i);
    Serial.println(f, 4);
    Serial.println(b ? "true" : "false");
}

void loop() { }
