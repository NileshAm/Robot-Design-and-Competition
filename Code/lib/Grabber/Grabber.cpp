#include "Grabber.h"

Grabber::Grabber(ServoMotor& grabServo, ServoMotor& liftServo, CurrentSensor& currentSensor)
    : _grabServo(grabServo), _liftServo(liftServo), _currentSensor(currentSensor)
{
    _grabSpeed = 50;     // % of servo speed for grab
    _liftAngle = 180;     // lift position
    _downAngle = 0;      // initial/rest position
    _openAngle = 50;      // fully open
    _closeAngle =130;    // fully closed limit
}

void Grabber::init() {
    // Set initial positions
    _grabServo.init(_openAngle/2);   
    _liftServo.init(_liftAngle/2);     
    
    home();

    // Start current sensor
    _currentSensor.begin();

    // LED for spike
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void Grabber::home(int grabberPos, int liftPos) {
    if (grabberPos == -1 && liftPos == -1)
    {
        grabberPos=_openAngle/2;
        liftPos=_liftAngle/2;
    }   
    _grabServo.moveSmooth(_openAngle/2, _openAngle,5, _grabSpeed);
    _liftServo.moveSmooth(_liftAngle/2, _liftAngle,5, _grabSpeed);

}



bool Grabber::grab() {
    for (float angle = _grabServo.getAngle(); angle <= _closeAngle; angle += 5) {
        _grabServo.writeAngle(angle);
        delay(_grabSpeed);

        if (_currentSensor.isSpike()) {
            digitalWrite(LED_BUILTIN, HIGH);
            int holdPos = angle + 5;
            if (holdPos > 140) holdPos = 140;
            _grabServo.writeAngle(holdPos);
            digitalWrite(LED_BUILTIN, LOW);
            return true;   // return boolean true
        }
    }

    return false;  // no spike detected
}



void Grabber::lift() {
    _liftServo.moveSmooth(_liftServo.getAngle(), _downAngle, 5, _grabSpeed);
}

void Grabber::release() {
    _grabServo.moveSmooth(_grabServo.getAngle(), _openAngle, 5, _grabSpeed);
}


void Grabber::reset() {
    _liftServo.moveSmooth(_liftServo.getAngle(), _liftAngle, 5, _grabSpeed);
}
