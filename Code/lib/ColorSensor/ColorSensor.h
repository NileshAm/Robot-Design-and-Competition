#pragma once
#include <Arduino.h>

enum ColorName {
    COLOR_UNKNOWN,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_BLACK,
    COLOR_WHITE
};

class ColorSensor {
public:
    ColorSensor(uint8_t s0Pin, uint8_t s1Pin, uint8_t s2Pin, uint8_t s3Pin, uint8_t outPin);

    void begin();
    void calibrate(uint16_t samples = 100);

    // Raw readings
    void readRaw(uint16_t &r, uint16_t &g, uint16_t &b);

    // Processed
    ColorName getColor();

    // Helpers
    bool isRed()   { return getColor() == COLOR_RED; }
    bool isGreen() { return getColor() == COLOR_GREEN; }
    bool isBlue()  { return getColor() == COLOR_BLUE; }
    bool isBlack() { return getColor() == COLOR_BLACK; }
    bool isWhite() { return getColor() == COLOR_WHITE; }

private:
    uint8_t _s0, _s1, _s2, _s3, _out;
    uint16_t _rMin, _gMin, _bMin;
    uint16_t _rMax, _gMax, _bMax;

    uint16_t _readChannel(bool c2, bool c3);
};
