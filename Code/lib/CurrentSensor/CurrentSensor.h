#pragma once
#include <Arduino.h>

class CurrentSensor {
public:
    CurrentSensor(uint8_t pin, float sensitivity, float spikeThreshold = 0.5);

    void begin(int samples = 100, int delayMs = 5);  // calibrate zero
    float read();     // returns current in A
    bool isSpike();   // detects sudden changes in current
    void setSpikeThreshold(float threshold);

private:
    uint8_t _pin;
    float _sensitivity;
    float _offset;        // zero-current voltage
    float _spikeThreshold;
    float _lastReading;
    float _deadband = 0.05; // ignore noise below 50 mA
};
