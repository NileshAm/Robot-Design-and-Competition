#include "ServoMotor.h"

ServoMotor::ServoMotor(uint8_t pin, int minUs, int maxUs, bool continuous)
: _pin(pin), _minUs(minUs), _maxUs(maxUs), _continuous(continuous) {}

void ServoMotor::attach() {
    if (!_srv.attached()) _srv.attach(_pin, _minUs, _maxUs);
}

void ServoMotor::init(int start) {
  attach();
    if (_continuous) {
        setPercent((int8_t)start);   // speed %
    } else {
        writeAngle((float)start);    // angle °
    }
}

void ServoMotor::_applyPulse(int us) {
    if (us < _minUs) us = _minUs;
    if (us > _maxUs) us = _maxUs;
    _srv.writeMicroseconds(us);
    _lastUs = us;
}

void ServoMotor::_applyAngle(float deg) {
    if (deg < 0.0f) deg = 0.0f;
    if (deg > 180.0f) deg = 180.0f;
    _srv.write((int)deg);
    _angle = deg;
    // approximate pulse for bookkeeping (optional)
    long us = map((long)deg, 0, 180, _minUs, _maxUs);
    _lastUs = (int)us;
}

void ServoMotor::writeAngle(float deg) {
    if (_continuous) return;     // ignore for continuous servos
    _applyAngle(deg);
}

void ServoMotor::setPercent(int8_t percent) {
    if (percent > 100) percent = 100;
    if (percent < -100) percent = -100;

    if (_continuous) {
        // 1500us stop; scale speed symmetrically to min/max
        int halfSpan = min(_maxUs - 1500, 1500 - _minUs);
        int us = 1500 + (int)((percent * halfSpan) / 100);
        _applyPulse(us);
    } else {
        // Map -100..100 -> 0..180°
        float deg = (percent + 100) * 0.9f;  // 200% range -> 180°
        _applyAngle(deg);
    }
}

void ServoMotor::writeMicroseconds(int us) {
    _applyPulse(us);
}
