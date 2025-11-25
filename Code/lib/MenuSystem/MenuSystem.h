#pragma once
#include "OLED.h"
#include "pushbutton.h"
#include "ColorSensor.h"
#include "Robot.h"        // <-- Added
#include "../src/Tasks/Task1/Task1.h"        // <-- Added
//#include "../src/Tasks/Task1/Task1.h"
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
               Robot &robot);      // <-- Robot added

    void begin();   
    void update();  

private:
    OLED &_oled;
    pushbutton &_btnUp;
    pushbutton &_btnDown;
    pushbutton &_btnSelect;
    ColorSensor &_sensor;
    Robot &_robot;                  // <-- Store robot reference

    String menuItems[8] = {
        "Calibrate Color",
        "Run Task 1",
        "Run Task 2",
        "Run Task 3",
        "Run Task 4",
        "Run Task 5",
        "Run Task 6",
        "Run All"
    };
    int menuCount = 8;
    int currentIndex = 0;

    void drawMenu();
    void runCalibration();
};
