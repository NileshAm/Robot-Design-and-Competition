#include "MenuSystem.h"
#include <Arduino.h>

MenuSystem::MenuSystem(OLED &oled,
                       pushbutton &btnUp,
                       pushbutton &btnDown,
                       pushbutton &btnSelect,
                       ColorSensor &sensor,
                       Robot &robot)
    : _oled(oled),
      _btnUp(btnUp),
      _btnDown(btnDown),
      _btnSelect(btnSelect),
      _sensor(sensor),
      _robot(robot)
{
}

void MenuSystem::begin() {
    _robot.setInterruptButton(_btnSelect);
    drawMenu();
}

// -------------------------------
// Update page according to scroll
// -------------------------------
void MenuSystem::updatePage() {
    if (currentIndex < pageStart)
        pageStart = currentIndex;

    else if (currentIndex >= pageStart + itemsPerPage)
        pageStart = currentIndex - (itemsPerPage - 1);
}

// -------------------------------
// Draw 3 items starting at pageStart
// -------------------------------
void MenuSystem::drawMenu() {
    _oled.clear();

    for (int i = 0; i < itemsPerPage; i++) {
        int itemIndex = pageStart + i;
        if (itemIndex >= menuCount) break;

        if (itemIndex == currentIndex)
            _oled.displayText("> " + menuItems[itemIndex], 0, i * 12);
        else
            _oled.displayText("  " + menuItems[itemIndex], 0, i * 12);
    }

    _oled.display();
}

// -------------------------------
// Allow running a task
// -------------------------------
void MenuSystem::runTask(const String &name, void (*fn)(Robot&)) {
    _oled.clear();
    _oled.displayCenteredText("Running " + name, 1);
    _oled.display();
    delay(800);

    if (fn != nullptr)
        fn(_robot);

    _oled.clear();
    _oled.displayCenteredText(name + " Done", 1);
    _oled.display();
    delay(1500);
}

// -------------------------------
// Calibration handler
// -------------------------------
void MenuSystem::runCalibration() {
    _oled.clear();
    _oled.displayCenteredText("Calibrating...", 1);
    _oled.display();

    _robot.calibrateIR();
    

    _oled.clear();
    _oled.displayCenteredText("Calibration OK", 1);
    _oled.display();
    delay(1500);
}
void MenuSystem::lineFollow() {

    _oled.clear();
    _oled.displayCenteredText("line following.....", 1);
    _oled.display();

    while (true)
    {
        _robot.followLine();
    }

    _oled.clear();
    _oled.displayCenteredText("Done", 1);
    _oled.display();
    delay(1500);
}

// -------------------------------
// Main update loop
// -------------------------------
void MenuSystem::update() {

    // ---- UP BUTTON (falling edge) ----
    if (_btnUp.stateChanged() == 1) {
        currentIndex--;
        if (currentIndex < 0) currentIndex = menuCount - 1;
        updatePage();
        drawMenu();
    }

    // ---- DOWN BUTTON (falling edge) ----
    if (_btnDown.stateChanged() == 1) {
        currentIndex++;
        if (currentIndex >= menuCount) currentIndex = 0;
        updatePage();
        drawMenu();
    }

    // ---- SELECT BUTTON (falling edge) ----
    if (_btnSelect.stateChanged() == 1) {

        switch (currentIndex) {

        case 0:
            runCalibration();
            break;

        case 1:
            lineFollow();
            break;

        case 2:
            runTask("Task 2", nullptr /* Task2::run */);
            break;

        case 3:
            runTask("Task 3", nullptr);
            break;

        case 4:
            runTask("Task 4", nullptr);
            break;

        case 5:
            runTask("Task 5", nullptr);
            break;

        case 6:
            runTask("Task 6", nullptr);
            break;

        case 7:
            runTask("All Tasks", nullptr);
            break;
        }

        drawMenu();
    }
}
