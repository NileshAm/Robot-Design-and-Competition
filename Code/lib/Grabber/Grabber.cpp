#include "Grabber.h"

Grabber::Grabber(ServoMotor& grabServo, ServoMotor& liftServo, CurrentSensor& currentSensor)
    : _grabServo(grabServo), _liftServo(liftServo), _currentSensor(currentSensor)
{
    _grabSpeed = 30;     // % of servo speed for grab
    _liftAngle = 90;     // lift position
    _downAngle = 0;      // initial/rest position
    _openAngle = 0;      // fully open
    _closeAngle = 90;    // fully closed limit
}


void Grabber::grab() {
    Serial.println("Grabbing object");
    _grabServo.attach();

    for (float angle = _openAngle; angle <= _closeAngle; angle += 1.0) {
        _grabServo.writeAngle(angle);
        delay(50); // smooth motion

        if (_currentSensor.isSpike()) {
            Serial.println("Spike detected: object grabbed.");
            break;
        }
    }
}


void Grabber::lift() {
    Serial.println("Lifting");
    _liftServo.attach();
    _liftServo.writeAngle(_liftAngle);
    delay(500);
}


void Grabber::release() {
    Serial.println("Releasing object");
    _grabServo.attach();
    _grabServo.writeAngle(_openAngle);
    delay(500);
}


void Grabber::reset() {
    Serial.println("Resetting grabber");
    _liftServo.attach();
    _liftServo.writeAngle(_downAngle);
    delay(500);

    _grabServo.attach();
    _grabServo.writeAngle(_openAngle);
    delay(500);
}
