#include "UltraSonic.h"

UltraSonic::UltraSonic(uint8_t echoPin, uint8_t trigPin)
: _echoPin(echoPin), _trigPin(trigPin) {}

void UltraSonic::init() {
    pinMode(_echoPin, INPUT);
    pinMode(_trigPin, OUTPUT); 
    digitalWrite(_trigPin, LOW);
}

unsigned long UltraSonic::_measurePulse() {
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);   
    digitalWrite(_trigPin, LOW);

    return pulseIn(_echoPin, HIGH, 30000UL); 
}

float UltraSonic::getDistance() {
    unsigned long duration = _measurePulse(); // microseconds
    return (duration / 2.0f) * 0.0343f;     
}
