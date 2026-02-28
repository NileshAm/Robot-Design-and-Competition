#pragma once
#include <Arduino.h>

class UltraSonic {
public:
    UltraSonic(uint8_t echoPin, uint8_t trigPin);

    void init();
    float getDistance(); // returns distance in cm

private:
    uint8_t _echoPin, _trigPin;
    unsigned long _measurePulse(); // internal pulse measurement
};
