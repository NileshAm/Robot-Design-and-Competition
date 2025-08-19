#include "PIDController.h"

float calculatePID(PID *pid, float error)
{
    // Proportional term
    float P_term = pid->P * error;

    // Integral term
    float I_term = pid->I * (pid->p_err + error);

    // Derivative term
    float D_term = pid->D * (error - pid->p_err);
    pid->p_err = error;

    // Total PID output
    return P_term + I_term + D_term;
}

void initPID(PID *pid, float P, float I, float D)
{
    pid->P = P;
    pid->I = I;
    pid->D = D;
    pid->p_err = 0.0f; // Initialize previous error to zero
}

void setP(PID *pid, float P)
{
    pid->P = P;
}
void setI(PID *pid, float I)
{
    pid->I = I;
}
void setD(PID *pid, float D)
{
    pid->D = D;
}
