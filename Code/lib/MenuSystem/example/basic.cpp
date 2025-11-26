#include <Arduino.h>

#include "OLED.h"
#include "pushbutton.h"
#include "ColorSensor.h"
#include "Tof.h"
#include "Motor.h"
#include "IRArray.h"
#include "Robot.h"
#include "MenuSystem.h"

// ------------------------------
// BUTTON PINS
// ------------------------------
#define BTN_UP     2
#define BTN_DOWN   3
#define BTN_SELECT 4

// ------------------------------
// HARDWARE OBJECTS
// ------------------------------
OLED oled;

pushbutton btnUp(BTN_UP);
pushbutton btnDown(BTN_DOWN);
pushbutton btnSelect(BTN_SELECT);

// Color sensors
ColorSensor grabberSensor;
ColorSensor boxColorSensor;

// Motors
Motor motorR(7, 8, 9,  2, 4, 600);
Motor motorL(7, 8, 9,  2, 4, 600);

// IR Array
IRArray irArr;

// TOF Sensors
Tof frontTof;
Tof leftTof;
Tof frontTopTof;
Tof grabberTof;

// ------------------------------
// ROBOT WITH *YOUR* CONSTRUCTOR
// ------------------------------
Robot robot(motorR,motorL,irArr,frontTof,leftTof,frontTopTof,grabberTof,grabberSensor,boxColorSensor,oled
);

// ------------------------------
// MENU
// ------------------------------
MenuSystem *menu;


// ------------------------------
void setup() {
    Serial.begin(115200);

    oled.begin();
    oled.clear();
    oled.displayCenteredText("Booting...", 2);
    delay(500);

    btnUp.init();
    btnDown.init();
    btnSelect.init();

    grabberSensor.begin();
    boxColorSensor.begin();

    irArr.init();

    frontTof.init();
    leftTof.init();
    frontTopTof.init();
    grabberTof.init();

    motorR.init();
    motorL.init();

    menu = new MenuSystem(oled, btnUp, btnDown, btnSelect, grabberSensor, robot);
    menu->begin();
}

// ------------------------------
void loop() {
    menu->update();
}
