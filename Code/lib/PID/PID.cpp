#include <Arduino.h>
#include "PID.h"

PID::PID(float kp, float ki, float kd, float setpoint)
: _kp(kp), _ki(ki), _kd(kd), _setpoint(setpoint),
  _prevError(0.0f), _integral(0.0f), _pid(0.0f), _lastMs(0UL) {}

float PID::compute(float current) {
    float error = _setpoint - current;

    unsigned long now = millis();
    float dt = 0.0f;
    if (_lastMs == 0UL) {
        _lastMs = now;
    } else {
        dt = (now - _lastMs) * 0.001f;   // ms -> s
        _lastMs = now;
    }

    if (dt > 0.0f) {
        _integral += error * dt;
        float derivative = (error - _prevError) / dt;
        _pid = _kp * error + _ki * _integral + _kd * derivative;
    } else {
        _pid = _kp * error;              // first call: no I/D yet
    }

    _prevError = error;
    return _pid;
}

void PID::setKp(float kp) { _kp = kp; }
void PID::setKi(float ki) { _ki = ki; }
void PID::setKd(float kd) { _kd = kd; }

void PID::updatePID(float kp, float ki, float kd) {
    _kp = kp; _ki = ki; _kd = kd;
}

void PID::setConstants(float kp, float ki, float kd) {
    updatePID(kp, ki, kd);
}

float PID::getP() const { return _kp; }
float PID::getI() const { return _ki; }
float PID::getD() const { return _kd; }

void PID::setSetpoint(float sp) { _setpoint = sp; }

void PID::updatePrevError(float prevError) { _prevError = prevError; }

float PID::getPID() const { return _pid; }

float PID::getPrevError() const { return _prevError; }

void PID::reset() {
    _integral = 0.0f;
    _prevError = 0.0f;
    _pid = 0.0f;
    _lastMs = 0UL;
}
