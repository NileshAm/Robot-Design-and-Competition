#pragma once

#include "OLED.h"
#include "pushbutton.h"
#include "ColorSensor.h"
#include "Robot.h"

#include "../../src/Tasks/Task1/Task1.h"
#include "../../src/Tasks/Traverse/Traverse.h" // added
#include "../src/Tasks/Task2/Task2.h"
#include "../src/Tasks/Task3/Task3.h"
#include "../src/Tasks/Task4/Task4.h"
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
    static const int menuCount = 18; // updated
    String menuItems[menuCount] = {
        "Test",
        "Line Follow",
        "Straight Line",
        "Ramp",
        "single Wall Follow",
        "Object detect",
        "Detect color",
        "Grab ball",
        "Grab box",
        "Calibrate IR",
        "Calibrate Color",
        "Grid run",
        "Run all",
        "TOF Debug",
        "IR Debug",
        "Run Task 1",
        "Test Bypass",
        "Traverse", // added
    };

    int currentIndex = 0;   // Selected entry
    int pageStart = 0;      // First item shown on screen

    void drawMenu();
    void updatePage();

    void test();
    void calibrateIR();
    void calibrateColor();
    void singleWallFollow();
    void objectDetect();
    void ramp();
    void detectColor();
    void debugTOF();
    void debugIR();
    void lineFollow();
    void straightLine();
    void gridRun();
    void garbBall();
    void GrabBox();
    void runTask1();
    void runTask(const String &name, void (*fn)(Robot&));
};
