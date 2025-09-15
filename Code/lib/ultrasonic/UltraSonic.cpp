#include "UltraSonic.h"

UltraSonic::UltraSonic(uint8_t echoPin, uint8_t trigPin)
: _echoPin(echoPin), _trigPin(trigPin) {} 

void UltraSoinc::init(){
    pinMode(_echoPin, INPUT);
    pinMode(_trigPin, INPUT);
    digitalWrite(_trigPin, LOW);
}

unsigned long UltraSonic::_measurePulse(){
    digitalWrite(_trigPin, HIGH);
    delayMicrosecond(10);
    digitalWrite(_trigPin, LOW);

    return pulseIn(_echoPin, HIGH, 30000UL);
}

float UltraSonic::getDistance(){
    unsigned long duration = _measurePulse();
    return (duration/2.0f)*0.0343f ;
}
    

