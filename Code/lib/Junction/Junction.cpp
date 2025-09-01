#include "Junction.h"

Junction::Junction(IRArray& ir):
    _ir(ir)
{}

bool Junction::isTurn(){
    return Junction::isRightTurn() || Junction::isLeftTurn();
}

bool Junction::isRightTurn(){
    bool out[8];
    _ir.digitalRead(out);

    if(out == (bool[]){0,0,0,1,1,1,1,1}){
        return true;
    }
    return false;
}

bool Junction::isLeftTurn(){
    bool out[8];
    _ir.digitalRead(out);

    if(out == (bool[]){1,1,1,1,1,0,0,0}){
        return true;
    }
    return false;
}

bool Junction::isTJunction(){
    bool out[8];
    _ir.digitalRead(out);

    if(out == (bool[]){1,1,1,1,1,1,1,1}){
        return true;
    }
    return false;
}