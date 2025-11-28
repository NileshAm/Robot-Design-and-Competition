#pragma once
#include <Arduino.h>
#include <Motor.h>
#include <IRArray.h>
#include <Tof.h>
#include <PID.h>
#include <Junction.h>
#include <ColorSensor.h>
#include <OLED.h>
#include <pushbutton.h>

class Robot {
    public:
        using TurnCallback = void (*)(Robot*);   // non-capturing lambda or plain function

        Robot(Motor& Motor_R, Motor& Motor_L, IRArray& IR_Arr, Tof& frontTof, Tof& leftTof, Tof& frontTopTof, Tof& rightTof, ColorSensor& grabberSensor, ColorSensor& boxColorSensor, OLED& oled);

        Motor& MotorR;        // dir1, dir2, pwm, encA, encB, ticks/rev
        Motor& MotorL;        // dir1, dir2, pwm, encA, encB, ticks/rev
        IRArray& ir;    // number of sensors, pins array
        Tof& frontTof;      // xshut, address, sda, scl
        Tof& leftTof;      // xshut, address, sda, scl
        Tof& frontTopTof;      // xshut, address, sda, scl
        Tof& rightTof;      // xshut, address, sda, scl //act as the right tof also
        ColorSensor& grabberSensor;
        ColorSensor& boxColorSensor;
        Junction junction;
        OLED& oled;

        void moveStraight();
        void moveStraight(float speed);
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
        void turn90(); // speed -100 to 100

        void turn(int angle);                                              // normal
        void turn(int angle, uint16_t cbEveryMs, TurnCallback cb);         // with callback
        void stop();

        void goTillTicks(long targetTicks);
        void goTillCM(float cm);

        void calibrateIR();

        void setInterruptButton(pushbutton& btn);
        bool isInterrupted();
        void setLineFollowerPID(float kp, float ki, float kd);
        void setWallFollowerPID(float kp, float ki, float kd);
        void setStraightLinePID(float kp, float ki, float kd);

    private:
        pushbutton* _interruptButton = nullptr;
        float _speed = 40;
        PID _straightLinePID;
        PID _lineFollowerPID;
        PID _wallFollowerPID;
        uint16_t _ticksPer360 = 1150;
        double _ticksPerDegree;

        void _turnCore(int angle, bool useCb, uint16_t cbEveryMs, TurnCallback cb);
};