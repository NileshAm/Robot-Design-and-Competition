#include "Robot.h"

Robot::Robot(Motor& Motor_R, Motor& Motor_L, IRArray& IR_Arr, Tof& Tof):
    MotorR(Motor_R),
    MotorL(Motor_L),
    ir(IR_Arr),
    tof1(Tof),

    _straightLinePID(0,0,0,0),
    _lineFollowerPID(0,0,0,3500),
    _wallFollowerPID(0,0,0,15) 
{
};


void Robot::turn90Clockwise(int speed) {
    MotorR.setSpeed(speed);
    MotorL.setSpeed(-speed);
    int ticks = 440;
    while (MotorR.getTicks() < ticks || MotorL.getTicks() > -ticks)
    {
        if (MotorR.getTicks() < ticks)
        {
            MotorR.setSpeed(0);
        }else
        {
            MotorL.setSpeed(0);
        }
        
    }
}

void Robot::turn(int anlge, int speed){

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