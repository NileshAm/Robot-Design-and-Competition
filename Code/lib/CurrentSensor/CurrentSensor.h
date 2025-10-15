#pragma once
#include <Arduino.h>

class CurrentSensor {
public:
    // pin = analog pin, sensitivity = V/A (e.g. 0.185 for ACS712 5A)
    // offset = voltage at 0A (usually 2.5V for ACS712)
    // spikeThreshold = minimum A difference to detect a spike
    CurrentSensor(uint8_t pin, float sensitivity, float offset = 2.5, float spikeThreshold = 0.5);

    float read();          // read current in Amperes
    bool isSpike();        // check if current spike detected
    void setOffset(float offset);
    void setSpikeThreshold(float threshold);

private:
    uint8_t _pin;
    float _sensitivity;
    float _offset;
    float _spikeThreshold;
    float _lastReading;
};
