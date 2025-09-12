#include <IRArray.h>
#include <Junction.h>

IRArray ir(8, (uint8_t[]){A0, A1, A2, A3, A4, A5, A6, A7});
Junction junction(ir);

void setup(){
    ir.init();
    ir.calibrate();
}

void loop(){
    if (junction.isTurn()){
        // do something
    }
    if (junction.isLeftTurn()){
        // do something
    }
    if (junction.isRightTurn()){
        // do something
    }
}
