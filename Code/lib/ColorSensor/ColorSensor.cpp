#include "ColorSensor.h"

ColorSensor::ColorSensor(uint8_t s0Pin, uint8_t s1Pin, uint8_t s2Pin, uint8_t s3Pin, uint8_t outPin)
: _s0(s0Pin), _s1(s1Pin), _s2(s2Pin), _s3(s3Pin), _out(outPin) {}

void ColorSensor::begin() {
    pinMode(_s0, OUTPUT);
    pinMode(_s1, OUTPUT);
    pinMode(_s2, OUTPUT);
    pinMode(_s3, OUTPUT);
    pinMode(_out, INPUT);

    digitalWrite(_s0, HIGH); // scaling freq
    digitalWrite(_s1, HIGH);
}

uint16_t ColorSensor::_readChannel(bool c2, bool c3) {
    digitalWrite(_s2, c2);
    digitalWrite(_s3, c3);
    // measure frequency (color intensity)
    return pulseIn(_out, LOW);
}

void ColorSensor::readRaw(uint16_t &r, uint16_t &g, uint16_t &b) {
    r = _readChannel(LOW, LOW);  // Red
    g = _readChannel(HIGH, HIGH); // Green
    b = _readChannel(LOW, HIGH); // Blue
}

void ColorSensor::calibrate(uint16_t samples) {
    uint16_t r, g, b;
    _rMin = _gMin = _bMin = 65535;
    _rMax = _gMax = _bMax = 0;

    for (uint16_t i = 0; i < samples; i++) {
        readRaw(r, g, b);
        _rMin = min(_rMin, r);
        _gMin = min(_gMin, g);
        _bMin = min(_bMin, b);
        _rMax = max(_rMax, r);
        _gMax = max(_gMax, g);
        _bMax = max(_bMax, b);
        delay(5);
    }
}

ColorName ColorSensor::getColor() {
    uint16_t r, g, b;
    readRaw(r, g, b);

    // crude decision thresholds (can be tuned)
    if (r < _rMin + 50 && g < _gMin + 50 && b < _bMin + 50) return COLOR_BLACK;
    if (r > _rMax - 50 && g > _gMax - 50 && b > _bMax - 50) return COLOR_WHITE;

    if (r < g && r < b) return COLOR_RED;
    if (g < r && g < b) return COLOR_GREEN;
    if (b < r && b < g) return COLOR_BLUE;

    return COLOR_UNKNOWN;
}
