#pragma once
#include <Arduino.h>

class pushbutton { // last-1 now-0 => ok
public:
    pushbutton(uint8_t buttonPin);
    void init();
    //void setlastButtonStatus();
    //int getlastButtonStatus();
    int buttonStatus();
    int stateChanged();

private:
    uint8_t _buttonPin;
    int _lastButtonState;
};
