#pragma once
#include <Arduino.h>
#include "ServoMotor.h"
#include "CurrentSensor.h"

class Grabber {
public:
    // Constructor takes 2 servo motors and a current sensor
    Grabber(ServoMotor& grabServo, ServoMotor& liftServo, CurrentSensor& currentSensor);

    // Main actions
    void grab();       // close grabber until spike detected
    void lift();       // lift arm
    void release();    // open grabber fully
    void reset();      // bring arm down and get ready for next grab

private:
    ServoMotor& _grabServo;
    ServoMotor& _liftServo;
    CurrentSensor& _currentSensor;

    // Internal helper parameters (you can tune)
    float _grabSpeed;      // % speed for closing grabber
    float _liftAngle;      // target angle for lift
    float _downAngle;      // rest position for lift
    float _openAngle;      // fully open angle for grabber
    float _closeAngle;     // fully closed angle (before spike)
};
