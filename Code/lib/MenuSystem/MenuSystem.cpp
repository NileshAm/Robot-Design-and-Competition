#include "MenuSystem.h"

MenuSystem::MenuSystem(OLED &oled, pushbutton &up, pushbutton &down, pushbutton &select)
: _oled(oled), _btnUp(up), _btnDown(down), _btnSelect(select), _currentSelection(0) {}

void MenuSystem::begin() {
    showMenu();
}

void MenuSystem::update() {
    if (_btnUp.stateChanged()) {
        _currentSelection--;
        if (_currentSelection < 0) _currentSelection = _menuLength - 1;
        showMenu();
    }

    if (_btnDown.stateChanged()) {
        _currentSelection++;
        if (_currentSelection >= _menuLength) _currentSelection = 0;
        showMenu();
    }

    if (_btnSelect.stateChanged()) {
        runAction(_currentSelection);
    }
}

void MenuSystem::showMenu() {
    _oled.clear();
    for (int i = 0; i < _menuLength; i++) {
        if (i == _currentSelection) {
            _oled.displayText("> " + String(_menuItems[i]), 0, i * 10, 1);
        } else {
            _oled.displayText(_menuItems[i], 10, i * 10, 1);
        }
    }
    _oled.display();
}

void MenuSystem::runAction(int selection) {
    _oled.clear();
    switch (selection) {
        case 0:
            _oled.displayCenteredText("Hello World!", 2);
            break;
        case 1:
            _oled.drawLine(0, 0, 127, 63);
            break;
        case 2:
            _oled.drawRect(10, 10, 80, 40);
            break;
        case 3:
            _oled.displayCenteredText("Cleared!", 2);
            break;
    }
    _oled.display();
    delay(2000);
    showMenu();
}
