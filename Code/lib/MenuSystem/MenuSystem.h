#pragma once

#include "OLED.h"
#include "pushbutton.h"
#include "ColorSensor.h"
#include "Robot.h"

#include "../../src/Tasks/Task1/Task1.h"
// #include "../src/Tasks/Task2/Task2.h"
// #include "../src/Tasks/Task3/Task3.h"
// #include "../src/Tasks/Task4/Task4.h"
// #include "../src/Tasks/Task5/Task5.h"
// #include "../src/Tasks/Task6/Task6.h"

class MenuSystem {
public:
    MenuSystem(OLED &oled,
               pushbutton &btnUp,
               pushbutton &btnDown,
               pushbutton &btnSelect,
               ColorSensor &sensor,
               Robot &robot);

    void begin();
    void update();

private:
    OLED &_oled;
    pushbutton &_btnUp;
    pushbutton &_btnDown;
    pushbutton &_btnSelect;
    ColorSensor &_sensor;
    Robot &_robot;

    // Visible menu lines per page on OLED
    static const int itemsPerPage = 5;

    // Menu items
    static const int menuCount = 9;
    String menuItems[menuCount] = {
        "Ramp",
        "Calibrate Color",
        "Line Follow",
        "Run Task 3",
        "Run Task 4",
        "Run Task 5",
        "Run Task 6",
        "Run All",
        "TOF Debug",
    };

    int currentIndex = 0;   // Selected entry
    int pageStart = 0;      // First item shown on screen

    void drawMenu();
    void updatePage();

    void calibrateIR();
    void ramp();
    void debugTOF();
    void debugIR();
    void lineFollow();
    void runTask(const String &name, void (*fn)(Robot&));
};
