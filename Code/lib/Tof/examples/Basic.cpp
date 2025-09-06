#include "Tof.h"

Tof tof1(7, 0x29, 20, 21); // xshut, address, sda, scl
Tof tof2(8, 0x30, 20, 21); // xshut, address, sda, scl

void setup(){
    Serial.begin(9600);
    tof1.disable();
    tof2.disable();

    tof1.init(10); // offset
    tof2.init(); // offset
}

void loop(){
    int r1 = tof1.readRange();
    int r2 = tof2.readRange();
    Serial.print("R1: ");
    Serial.print(r1);
    Serial.print(" R2: ");
    Serial.println(r2);
    delay(100);
}