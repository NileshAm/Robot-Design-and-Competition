#pragma once
#include <Arduino.h>

class Tof
{
private:
    uint8_t _xshut;
    uint8_t _address;
    uint8_t _sda;
    uint8_t _scl;
    Adafruit_VL6180X _sensor;

public:
    Tof(uint8_t xshut, int address, uint8_t sda, uint8_t scl);
    void disable();
    void enable();
    void init(int offset=0);
    int readRange();
};

