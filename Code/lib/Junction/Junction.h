#pragma once
#include <IRArray.h>

class Junction
{
private:
    IRArray& _ir;

public:
    Junction(IRArray& ir);

    bool isRightTurn();
    bool isLeftTurn();
    bool isTurn();
    bool isTJunction();
    bool isAllWhite();
    bool isAllBlack();
    bool isLine();

};
