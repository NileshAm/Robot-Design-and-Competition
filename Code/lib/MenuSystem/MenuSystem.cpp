#include "MenuSystem.h"
#include <Arduino.h>

MenuSystem::MenuSystem(OLED &oled, pushbutton &btnUp, pushbutton &btnDown, pushbutton &btnSelect, ColorSensor &sensor)
    : _oled(oled), _btnUp(btnUp), _btnDown(btnDown), _btnSelect(btnSelect), _sensor(sensor) {}

void MenuSystem::begin() {
    drawMenu();
}

void MenuSystem::update() {
    // Navigate Up
    if (_btnUp.stateChanged()) {
        currentIndex--;
        if (currentIndex < 0) currentIndex = menuCount - 1;
        drawMenu();
    }

    // Navigate Down
    if (_btnDown.stateChanged()) {
        currentIndex++;
        if (currentIndex >= menuCount) currentIndex = 0;
        drawMenu();
    }

    // Select button
    if (_btnSelect.stateChanged()) {
        _oled.begin();
        _oled.clear();

         if (currentIndex == 0) { // Option 1: Calibration 
                _oled.clear();
                _oled.displayText("White", 0, 0, 1);
                _oled.display();
                delay(2000);
                //_sensor.calibrate(150, true);
                _oled.clear();
                _oled.displayText("Black", 0, 0, 1);
                _oled.display();
                delay(2000);
                //_sensor.calibrate(150, false);
                _oled.clear();
                _oled.displayText("Done", 0, 0, 1);
                _oled.display();
                delay(2000);

            //runCalibration();

            drawMenu();
        } else if (currentIndex == 1) { // Option 2
            _oled.displayCenteredText("Option 2 selected", 1);
            _oled.display();
            Serial.println("Option 2 selected");
            delay(2000);
            drawMenu();
        } else if (currentIndex == 2) { // Option 3
            _oled.displayCenteredText("Option 3 selected", 1);
            _oled.display();
            Serial.println("Option 3 selected");
            delay(2000);
            drawMenu();
        }
    }
}

void MenuSystem::drawMenu() {
    _oled.clear();
    for (int i = 0; i < menuCount; i++) {
        String text = (i == currentIndex ? "> " : "  ") + menuItems[i];
        _oled.displayText(text, 0, i * 10, 1);
    }
    _oled.display();
}

void MenuSystem::runCalibration() {
    _oled.begin();
    _oled.clear();
    _oled.displayText("White", 0, 0, 1);
    _oled.display();
    delay(3000);
    //_sensor.calibrate(150, true);

    _oled.clear();
    _oled.displayText("Black", 0, 0, 1);
    _oled.display();
    delay(3000);
    //_sensor.calibrate(150, false);

    _oled.clear();
    _oled.displayText("Done", 0, 0, 1);
    _oled.display();
    Serial.println("Calibration complete!");
    delay(2000);
}
