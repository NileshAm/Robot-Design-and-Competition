#pragma once
#include <Arduino.h>

class UltraSonic{

public:
    UltraSonic(uint8_t echoPin, uint8_t trigPiin);

    void init();
    void getDistance():

private:
    uint8_t _echoPin, _trigPin;
    unsigned long _measurePulse():
}