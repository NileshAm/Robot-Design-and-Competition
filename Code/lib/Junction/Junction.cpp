#include "Junction.h"
#include <Utils.h>

Junction::Junction(IRArray& ir):
    _ir(ir)
{}

bool Junction::isTurn(){
    return Junction::isRightTurn() || Junction::isLeftTurn();
}

bool Junction::isRightTurn(){
    bool out[8];
    _ir.readDigital(out);
    return pack8(out) == 0b00011111 || pack8(out) == 0b00001111;
}

bool Junction::isLeftTurn(){
    bool out[8];
    _ir.readDigital(out);
    return pack8(out) == 0b11111000 || pack8(out) == 0b11110000;
}

bool Junction::isTJunction(){
    bool out[8];
    _ir.readDigital(out);    
    return pack8(out) == 0b11111111;
}

bool Junction::isAllBlack(){
    bool out[8];
    _ir.readDigital(out);
    
    return pack8(out) == 0b00000000;
}

bool Junction::isAllWhite(){
    bool out[8];
    _ir.readDigital(out);
    
    return pack8(out) == 0b11111111;
}
bool Junction::isLine(){
    bool out[8];
    _ir.readDigital(out);

    int c = 0;
    for (bool v : out) c += v;
    return c == 1 || c == 2 || c==3 || c==4;
}