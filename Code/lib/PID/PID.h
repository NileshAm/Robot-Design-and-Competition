#pragma once

class PID {
public:
    PID(float kp, float ki, float kd, float setpoint);

    float compute(float current);            // returns PID output

    // Update gains (individually or all at once)
    void setKp(float kp);
    void setKi(float ki);
    void setKd(float kd);
    void updatePID(float kp, float ki, float kd);

    // Other utilities
    void setSetpoint(float sp);
    void updatePrevError(float prevError);

    float getPID() const;         // last output
    float getPrevError() const;   // last error used

    void reset();                 // clears integral, prev error, output

private:
    float _kp, _ki, _kd;
    float _setpoint;

    float _prevError;
    float _integral;
    float _pid;

    unsigned long _lastMs;
};
