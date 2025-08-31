#pragma once
#include <Arduino.h>
#include <Motor.h>
#include <IRArray.h>
#include <Tof.h>
#include <PID.h>

class Robot {
    public:
        Robot(Motor& Motor_R, Motor& Motor_L, IRArray& IR_Arr, Tof& Tof);

        Motor& MotorR, MotorL;        // dir1, dir2, pwm, encA, encB, ticks/rev
        IRArray& ir;    // number of sensors, pins array
        Tof& tof1;      // xshut, address, sda, scl

        void moveStraight();
        void followLine();
        void followWall();

        
        /**
         * @brief Rotate the robot approximately 90 degrees in place.
         *
         * Commands the robot to perform an in-place rotation of roughly 90 degrees.
         * The direction of rotation is determined by the sign of the speed parameter:
         * positive values rotate clockwise, negative values rotate counter-clockwise.
         *
         * @param speed Rotation speed and direction in the range [-100, 100].
         *              Positive => clockwise, Negative => counter-clockwise.
         *              The magnitude controls the rotation rate (0 = no rotation).
         *
         * @note Values are expected to be clamped to the supported range by the caller
         *       or implementation. Exact rotation angle, timing, and behavior under
         *       load or obstruction are implementation-dependent.
         */
        void turn90Clockwise(int speed); // speed -100 to 100

        void turn(int angle, int speed);

    private:
        float _speed = 50;
        PID _straightLinePID;
        PID _lineFollowerPID;
        PID _wallFollowerPID;
};