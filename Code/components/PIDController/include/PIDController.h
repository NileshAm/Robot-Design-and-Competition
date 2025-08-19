#ifndef PIDCONTROLER_H
#define PIDCONTROLER_H

typedef struct
{
    float P;
    float I;
    float D;

    float p_err;
} PID;

float calculatePID(PID *pid, float error);

void initPID(PID *pid, float P, float I, float D);

void setP(PID *pid, float P);
void setI(PID *pid, float I);
void setD(PID *pid, float D);

#endif