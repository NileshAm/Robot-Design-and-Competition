#include "CurrentSensor.h"

CurrentSensor::CurrentSensor(uint8_t pin, float sensitivity, float spikeThreshold)
    : _pin(pin), _sensitivity(sensitivity), _spikeThreshold(spikeThreshold),
      _lastReading(0), _offset(0.00) { // default 0A voltage
    pinMode(_pin, INPUT);
}

void CurrentSensor::begin(int samples, int delayMs) {
    // Auto-calibrate zero-current offset
    long sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += analogRead(_pin);
        delay(delayMs);
    }
    float adcAvg = sum / (float)samples;
    _offset = adcAvg * 5.0 / 1023.0; // convert to voltage
}

float CurrentSensor::read() {
    const int samples = 10;   // average to reduce noise
    long sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += analogRead(_pin);
        delayMicroseconds(200);
    }
    float avg = sum / (float)samples;

    float voltage = (avg / 1023.0) * 5.0;
    float current = (voltage - _offset) / _sensitivity;

    // Apply deadband to ignore small noise
    if (abs(current) < _deadband) current = 0;

    return current;
}

bool CurrentSensor::isSpike() {
    float current = read();
    // float diff = fabs(current - _lastReading);
    bool spike = fabs(current) > _spikeThreshold;
    _lastReading = current;
    return spike;
}

void CurrentSensor::setSpikeThreshold(float threshold) {
    _spikeThreshold = threshold;
}
