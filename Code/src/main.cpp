#include <Arduino.h>
#include "OLED.h"

OLED oled;

void setup() {
    Serial.begin(9600);

    if (!oled.begin()) {
        Serial.println("OLED init failed!");
        while (1);
    }

    oled.clear();
    oled.displayText("Hello World!", 0, 0, 1);
    oled.display();
    delay(2000);

    oled.clear();
    oled.displayCenteredText("Centered!", 2);
    oled.display();
    delay(2000);

    oled.clear();
    oled.drawLine(0, 0, 127, 63);
    oled.drawRect(10, 10, 50, 30);
    oled.display();
}

void loop() {
    // Nothing for now
}
