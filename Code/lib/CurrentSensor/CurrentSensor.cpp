#include "CurrentSensor.h"

CurrentSensor::CurrentSensor(uint8_t pin, float sensitivity, float offset, float spikeThreshold)
    : _pin(pin), _sensitivity(sensitivity), _offset(offset),
      _spikeThreshold(spikeThreshold), _lastReading(0) {
    pinMode(_pin, INPUT);
}

float CurrentSensor::read() {
    const int samples = 10;   // average to reduce noise
    long sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += analogRead(_pin);
        delayMicroseconds(200);
    }
    float avg = sum / (float)samples;

    // Convert ADC reading (10-bit, 5V reference)
    float voltage = (avg / 1023.0) * 5.0;

    // Convert voltage to current (A)
    float current = (voltage - _offset) / _sensitivity;

    return current;
}

bool CurrentSensor::isSpike() {
    float current = read();
    float diff = fabs(current - _lastReading);
    bool spike = diff > _spikeThreshold;
    _lastReading = current;
    return spike;
}

void CurrentSensor::setOffset(float offset) {
    _offset = offset;
}

void CurrentSensor::setSpikeThreshold(float threshold) {
    _spikeThreshold = threshold;
}
