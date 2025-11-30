#pragma once
#include <Arduino.h>
#include <Servo.h>

class ServoMotor {
public:
    // If continuous = false -> positional servo (0..180°)
    // If continuous = true  -> continuous servo (speed via percent, 1500us stop)
    ServoMotor(uint8_t pin, int minUs = 500, int maxUs = 2500, bool continuous = false);

    // Positional: start = start angle (°). Continuous: start = % speed (-100..100), 0 = stop
    void init(int start = 90);

    // ---- Control ----
    void writeAngle(float deg);                 // 0..180° (positional only)
    void setPercent(int8_t percent);            // -100..100 (positional->angle, continuous->speed)
    void writeMicroseconds(int us);             // direct pulse write

    // ---- State ----
    float getAngle() const            { return _angle; }   // last angle commanded
    int   getPulseUs() const          { return _lastUs; }  // last pulse us

    void moveSmooth(int start, int end, int step, int d = 50);

    // ---- Lifecycle ----
    void attach();                                     // re-attach using stored params
    void detach()                { _servo.detach(); }

private:
    Servo   _servo;
    uint8_t _pin;
    int     _minUs, _maxUs;
    bool    _continuous;

    float   _angle = 90.0f;        // cached angle (positional)
    int     _lastUs = 1500;        // cached pulse width

    void _applyAngle(float deg);   // internal helpers
    void _applyPulse(int us);
};
