#pragma once
#include "OLED.h"
#include "pushbutton.h"
#include "ColorSensor.h"

class MenuSystem {
public:
    MenuSystem(OLED &oled, pushbutton &btnUp, pushbutton &btnDown, pushbutton &btnSelect, ColorSensor &sensor);

    void begin();   // Display initial menu
    void update();  // Handle button presses and run actions

private:
    OLED &_oled;
    pushbutton &_btnUp;
    pushbutton &_btnDown;
    pushbutton &_btnSelect;
    ColorSensor &_sensor;

    // Hardcoded menu items
    String menuItems[3] = {"Calibrate Color", "Calibrate IR", "Run All"};
    int menuCount = 3;
    int currentIndex = 0;

    void drawMenu();         // Draw menu on OLED
    void runCalibration();   // Run color sensor calibration
};
