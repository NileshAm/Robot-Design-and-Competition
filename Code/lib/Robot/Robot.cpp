#include "Robot.h"

Robot::Robot(Motor& Motor_R, Motor& Motor_L, IRArray& IR_Arr, Tof& Tof):
    MotorR(Motor_R),
    MotorL(Motor_L),
    ir(IR_Arr),
    tof1(Tof),

    _straightLinePID(0.1,0.2,0,0),
    _lineFollowerPID(0,0,0,3500),
    _wallFollowerPID(0,0,0,15)

{
    _ticksPerDegree = (double)_ticksPer360 / 360;
};


void Robot::turn90() {
    Robot::turn(90);
}

void Robot::turn(int angle) {
  // reset encoders
  MotorL.resetTicks();
  MotorR.resetTicks();

  // target ticks (always positive)
  long target = lround(fabsf(_ticksPerDegree * (float)angle));

  // direction: +angle => L forward, R backward (swap if your robot turns opposite)
  int dir = (angle >= 0) ? 1 : -1;

  MotorL.setSpeed(_speed * dir);
  MotorR.setSpeed(-_speed * dir);

  // safety timeout (optional)
  const unsigned long TIMEOUT_MS = 5000;
  unsigned long t0 = millis();

  while (true) {
    long L = labs(MotorL.getTicks());  // absolute progress
    long R = labs(MotorR.getTicks());

    if (L >= target) MotorL.setSpeed(0);
    if (R >= target) MotorR.setSpeed(0);

    if (L >= target && R >= target) break;
    if (millis() - t0 > TIMEOUT_MS)   break;  // safety exit
    // (optional) small pause to ease serial spam
    // delay(1);
  }

  MotorL.setSpeed(0);
  MotorR.setSpeed(0);
}


void Robot::moveStraight() {
    int error = MotorR.getTicks() - MotorL.getTicks();
    int correction = (int)_straightLinePID.compute((float)error);
    MotorR.setSpeed(_speed + correction);
    MotorL.setSpeed(_speed - correction);
}

void Robot::followLine(){
    int error = ir.weightedSum();
    int correction = (int)_straightLinePID.compute((float)error);
    MotorR.setSpeed(_speed + correction);
    MotorL.setSpeed(_speed - correction);
}

void Robot::followWall(){
    int error = tof1.readRange();
    int correction = (int)_straightLinePID.compute((float)error);
    MotorR.setSpeed(_speed + correction);
    MotorL.setSpeed(_speed - correction);
}