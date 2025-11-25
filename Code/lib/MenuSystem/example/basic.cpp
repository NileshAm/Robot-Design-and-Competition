#include <Arduino.h>
#include "OLED.h"
#include "pushbutton.h"
#include "ColorSensor.h"
#include "MenuSystem.h"

// OLED
OLED oled;

// Buttons
pushbutton btnUp(2);
pushbutton btnDown(3);
pushbutton btnSelect(4);

// Color sensor
ColorSensor sensor(4, 5, 6, 7, 8);

// Menu system
MenuSystem menu(oled, btnUp, btnDown, btnSelect, sensor);

void setup() {
    Serial.begin(9600);

    // Initialize buttons
    btnUp.init();
    btnDown.init();
    btnSelect.init();

    // Initialize OLED
    if (!oled.begin()) {
        Serial.println("OLED init failed!");
        while (1);
    }

    // Initialize sensor
    sensor.begin();
    Serial.println("sensor begin done");
    // Display menu
    menu.begin();
    Serial.println("menu begin done");
}

void loop() {
    menu.update();
    delay(100); // debounce
}
