#include "pushbutton.h"

pushbutton::pushbutton(uint8_t buttonPin)
:_buttonPin(buttonPin),_lastButtonState(LOW) {}


void pushbutton::init(){
    pinMode(_buttonPin, INPUT_PULLUP);
}

// int pushbutton::getlastButtonStatus(){
//     return _lastButtonState;
// }

int pushbutton::buttonStatus(){
    return digitalRead(_buttonPin);
}

// void pushbutton::setlastButtonStatus(){
//     _lastButtonState = buttonStatus();
// }

int pushbutton::stateChanged(){
    int currentState = buttonStatus();
    int isStateChanged = 0;
    if(_lastButtonState == HIGH && currentState == LOW){
        isStateChanged = 1;
    }
    _lastButtonState = currentState;

    return isStateChanged ;
}
