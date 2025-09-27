#pragma once
#include <Arduino.h>
#include "OLED.h"
#include "pushbutton.h"

class MenuSystem {
public:
    MenuSystem(OLED &oled, pushbutton &up, pushbutton &down, pushbutton &select);

    void begin();
    void update();

private:
    OLED &_oled;
    pushbutton &_btnUp;
    pushbutton &_btnDown;
    pushbutton &_btnSelect;

    void showMenu();
    void runAction(int selection);

    int _currentSelection;
    static const int _menuLength = 4;
    const char* _menuItems[_menuLength] = {
        "Show Hello",
        "Draw Line",
        "Draw Rectangle",
        "Clear Screen"
    };
};
