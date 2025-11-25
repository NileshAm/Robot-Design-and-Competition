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
      _robot(robot) {}         // <-- assign robot

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

        if (currentIndex == 0) { 
            // ===== Option 1: Color Calibration =====
            runCalibration();
            drawMenu();
        } 
        
        else if (currentIndex == 1) { 
            // ===== Option 2: RUN TASK 1 =====
            _oled.clear();
            _oled.displayCenteredText("Running Task 1...", 1);
            _oled.display();
            delay(1000);

            Task1::run(_robot);  // <<--- CALL TASK 1 HERE

            _oled.clear();
            _oled.displayCenteredText("Task 1 Done", 1);
            _oled.display();
            delay(2000);

            drawMenu();
        } 

            else if (currentIndex == 2) { 
            // ===== Option 3: RUN TASK 2 =====
            _oled.clear();
            _oled.displayCenteredText("Running Task 2...", 1);
            _oled.display();
            delay(1000);

            //Task2::run(_robot);  // <<--- CALL TASK 1 HERE

            _oled.clear();
            _oled.displayCenteredText("Task 2 Done", 1);
            _oled.display();
            delay(2000);

            drawMenu();
        } 

            else if (currentIndex == 3) { 
            // ===== Option 4: RUN TASK 3 =====
            _oled.clear();
            _oled.displayCenteredText("Running Task 3...", 1);
            _oled.display();
            delay(1000);

            //Task3::run(_robot);  // <<--- CALL TASK 1 HERE

            _oled.clear();
            _oled.displayCenteredText("Task 3 Done", 1);
            _oled.display();
            delay(2000);

            drawMenu();
        } 

                else if (currentIndex == 4) { 
            // ===== Option 5: RUN TASK 4 =====
            _oled.clear();
            _oled.displayCenteredText("Running Task 4...", 1);
            _oled.display();
            delay(1000);

            //Task4::run(_robot);  // <<--- CALL TASK 1 HERE

            _oled.clear();
            _oled.displayCenteredText("Task 1 Done", 1);
            _oled.display();
            delay(2000);

            drawMenu();
        } 

            else if (currentIndex == 5) { 
            // ===== Option 6: RUN TASK 5 =====
            _oled.clear();
            _oled.displayCenteredText("Running Task 1...", 1);
            _oled.display();
            delay(1000);

            //Task5::run(_robot);  // <<--- CALL TASK 1 HERE

            _oled.clear();
            _oled.displayCenteredText("Task 1 Done", 1);
            _oled.display();
            delay(2000);

            drawMenu();
        } 

            else if (currentIndex == 6) { 
            // ===== Option 7: RUN TASK 6 =====
            _oled.clear();
            _oled.displayCenteredText("Running Task 6...", 1);
            _oled.display();
            delay(1000);

            //Task1::run(_robot);  // <<--- CALL TASK 1 HERE

            _oled.clear();
            _oled.displayCenteredText("Task 1 Done", 1);
            _oled.display();
            delay(2000);

            drawMenu();
        } 
        
        else if (currentIndex == 2) { 
            // ===== Option 3: Run All =====
            _oled.displayCenteredText("Run All Selected", 1);
            _oled.display();
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
    _oled.clear();
    _oled.displayText("White", 0, 0, 1);
    _oled.display();
    delay(3000);

    _oled.clear();
    _oled.displayText("Black", 0, 0, 1);
    _oled.display();
    delay(3000);

    _oled.clear();
    _oled.displayText("Done", 0, 0, 1);
    _oled.display();
    Serial.println("Calibration complete!");
    delay(2000);
}
