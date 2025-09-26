#include "Robot.h"

Robot::Robot(Motor& Motor_R, Motor& Motor_L, IRArray& IR_Arr, Tof& Tof):
    MotorR(Motor_R),
    MotorL(Motor_L),
    ir(IR_Arr),
    tof1(Tof),
    junction(IR_Arr),

    _straightLinePID(0.1,0.2,0,0),
    _lineFollowerPID(0.25,0.00,0,70),
    _wallFollowerPID(0.25,0.1,0,90)

{
    _ticksPerDegree = (double)_ticksPer360 / 360;
};

void Robot::stop(){
    MotorL.setSpeed(0);
    MotorR.setSpeed(0);
}

void Robot::turn90() {
    Robot::turn(90);
}

void Robot::turn(int angle) {
  _turnCore(angle, /*useCb=*/false, 0, nullptr);
}

void Robot::turn(int angle, uint16_t cbEveryMs, TurnCallback cb) {
  _turnCore(angle, /*useCb=*/true, cbEveryMs ? cbEveryMs : 1, cb);
}

void Robot::_turnCore(int angle, bool useCb, uint16_t cbEveryMs, TurnCallback cb) {
  if (angle == 0) return;

  MotorL.resetTicks();
  MotorR.resetTicks();

  const long target = lround(fabsf(_ticksPerDegree * (float)angle));
  const int dir = (angle >= 0) ? 1 : -1;

  MotorL.setSpeed(_speed * dir);
  MotorR.setSpeed(-_speed * dir);

  unsigned long nextCb = millis() + cbEveryMs;
  const unsigned long TIMEOUT_MS = 8000;
  const unsigned long t0 = millis();

  while (true) {
    const unsigned long now = millis();

    if (useCb && (long)(now - nextCb) >= 0) {
      if (cb) cb(this);                 // e.g., [](Robot* r){ r->ir.updateSensors(); }
      nextCb += cbEveryMs;
    }

    long L = labs(MotorL.getTicks());
    long R = labs(MotorR.getTicks());

    if (L >= target) MotorL.setSpeed(0);
    if (R >= target) MotorR.setSpeed(0);

    if ((L >= target && R >= target) || (now - t0 > TIMEOUT_MS)) break;
  }

  MotorL.setSpeed(0);
  MotorR.setSpeed(0);
}

void Robot::moveStraight(float speed) {
  int error = MotorR.getTicks() - MotorL.getTicks();
  int correction = (int)_straightLinePID.compute((float)error);
  MotorR.setSpeed(speed + correction);
  MotorL.setSpeed(speed - correction);

}
void Robot::moveStraight() {
    moveStraight(_speed);
}

void Robot::followLine(){
    int error = ir.weightedSum();
    error = constrain(error, 0, 130);
    int correction = (int)_lineFollowerPID.compute((float)error);

    MotorR.setSpeed(_speed - correction);
    MotorL.setSpeed(_speed + correction);
}

void Robot::followWall(){
    int error = tof1.readRange();
    int correction = (int)_wallFollowerPID.compute((float)error);
    MotorR.setSpeed(_speed - correction);
    MotorL.setSpeed(_speed + correction);
}

void Robot::goTillTicks(long targetTicks){
    MotorL.resetTicks();
    MotorR.resetTicks();

    while ((MotorL.getTicks() + MotorR.getTicks())/2 < targetTicks)
    {
        Robot::moveStraight();
        delay(1);
    }
    MotorL.setSpeed(0);
    MotorR.setSpeed(0);
    
}

void Robot::goTillCM(float cm){
    long targetTicks = lround((cm / (3.14159 * MotorL.getWheelDiameter())) * MotorL.getTicksPerRev());
    Robot::goTillTicks(targetTicks);
}

void Robot::calibrateIR(){
    int angles[5] = {45, -90, 90, -90, 45};
    for (int i = 0; i < 5; i++)
    {
        Robot::turn(angles[i], 10, [](Robot *r)
                    { r->ir.updateSensors(); });
    }
    ir.calibrate();
}