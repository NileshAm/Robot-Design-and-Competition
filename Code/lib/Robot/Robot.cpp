#include "Robot.h"

Robot::Robot(Motor& Motor_R, Motor& Motor_L, IRArray& IR_Arr, Tof& frontTof, Tof& leftTof, Tof& leftTof2, Tof& frontTopTof, Tof& rightTof, ColorSensor& grabberSensor, ColorSensor& boxColorSensor, Grabber& grabber, OLED& oled, MPU6050 &imu) :
    MotorR(Motor_R),
    MotorL(Motor_L),
    ir(IR_Arr),
    frontTof(frontTof),
    leftTof(leftTof),
    leftTof2(leftTof2),
    frontTopTof(frontTopTof),
    rightTof(rightTof),
    grabberSensor(grabberSensor),
    boxColorSensor(boxColorSensor),
    grabber(grabber),
    junction(IR_Arr),
    oled(oled),
    imu(imu),

_straightLinePID(0.04, 0, 0, 0),
_straightLineGyroPID(0.5, 0, -0.1, 0),
_rampPID(1, 0, 0, 0),
// _lineFollowerPID(0.02,0,-0.6,1400),      40% pwm
// _lineFollowerPID(0.012,0.004,-0.025,1400),
_lineFollowerPID(0.02, -0.005, 0, 950),
// _singleWallFollowerPID(2,0.15,0,0),
// _singleWallDistancePID(0.5,0,0.05,100),
_singleWallFollowerPID(1, 0.1, 0, 0),
_singleWallDistancePID(0.75, 0, 0, 120),
_doubleWallFollowerPID(0.25, 0.1, 0, 90),
_turnPID(1, 0, 0, 0)
{
    _ticksPerDegree = (double)_ticksPer360 / 360;
};

void Robot::stop()
{
    MotorL.setSpeed(0);
    MotorR.setSpeed(0);
}
void Robot::brake()
{
    MotorL.brake();
    MotorR.brake();
}

void Robot::turn90(bool clockwise)   
{
    goTillCM(7);
    if (clockwise)
    {
        turn(90);
    }
    else
    {
        turn(-90);
    }
    goTillCM(-5);
}
void Robot::turnLeft()   
{
    Robot::turn90(false);
    //Robot::centerOnLine();
}
void Robot::turnRight()   
{
    Robot::turn90(true);
}

void Robot::turn(int angle)
{
    _turnCore(angle);
}

// void Robot::turn(int angle, uint16_t cbEveryMs, TurnCallback cb)
// {
//     _turnCore(angle, /*useCb=*/true, cbEveryMs ? cbEveryMs : 1, cb);
// }

// void Robot::_turnCore(int angle, bool useCb, uint16_t cbEveryMs, TurnCallback cb)
// {
//     PID turnPID(0.15, 0, 0, 0);
//     if (angle == 0)
//         return;

//     MotorL.resetTicks();
//     MotorR.resetTicks();

//     const long target = lround(fabsf(_ticksPerDegree * (float)angle * 1));
//     const int dir = (angle >= 0) ? 1 : -1;

//     MotorL.setSpeed(30 * dir);
//     MotorR.setSpeed(-30 * dir);

//     unsigned long nextCb = millis() + cbEveryMs;
//     const unsigned long TIMEOUT_MS = 8000;
//     const unsigned long t0 = millis();

//     while (labs(MotorL.getTicks()) < target || labs(MotorR.getTicks()) < target)
//     {
//         const unsigned long now = millis();

//         if (useCb && (long)(now - nextCb) >= 0)
//         {
//                 if (cb)
//                     cb(this); // e.g., [](Robot* r){ r->ir.updateSensors(); }
//                 nextCb += cbEveryMs;
//         }

//         long L = labs(MotorL.getTicks());
//         long R = labs(MotorR.getTicks());

//         int error = L - R;
//         int correction = (int)turnPID.compute((float)error);

//         // if (L >= target)
//         //     MotorL.setSpeed(0);
//         // if (R >= target)
//         //     MotorR.setSpeed(0);

//         // if ((L >= target && R >= target) || (now - t0 > TIMEOUT_MS))
//         //     break;
//         float speedFactor = 1.1;
//         MotorR.setSpeed((-30 * dir * speedFactor) - correction);
//         MotorL.setSpeed((30 * dir * speedFactor) + correction);
//     }
            
//     brake();
// }
            

void Robot::_turnCore(int angle)
{
    int initAngle = imu.getYaw();
    if (angle == 0)
        return;

    // Reset encoders so we can measure progress & drift
    MotorL.resetTicks();
    MotorR.resetTicks();

    // Positive angle => turn left (L forward, R backward)
    const int dir = (angle >= 0) ? 1 : -1;

    // How many encoder ticks we expect per degree
    const long targetTicks = lroundf(fabsf(_ticksPerDegree * (float)angle));

    const int maxTurnSpeed      = 40;
    const int minEffectiveSpeed = 30;
    const float ticksTolFactor  = 1.1f;    // stop when we reach ~98% of target
    const long  TIMEOUT_MS      = 5000;
    const unsigned long t0      = millis();

    while (true)
    {
        long L = labs(MotorL.getTicks());
        long R = labs(MotorR.getTicks());

        // Average ticks is a proxy for rotation progress
        long avgTicks = (L + R) / 2;

        // Error in "how much turn is left"
        long remaining = targetTicks - avgTicks;
        float error = (float)remaining;

        // PID outputs a scalar "spin speed"
        float u = _turnPID.compute(error);

        // Convert to signed speed (dir handles CW/CCW)
        int mag = (int)fabsf(u);
        if (mag > maxTurnSpeed) mag = maxTurnSpeed;

        // Make sure it's strong enough to move
        if (mag > 0 && mag < minEffectiveSpeed)
            mag = minEffectiveSpeed;

        // Enforce equal absolute speeds: |L| == |R|
        int leftSpeed  =  dir * mag;
        int rightSpeed = -dir * mag;

        MotorL.setSpeed(leftSpeed);
        MotorR.setSpeed(rightSpeed);

        bool reached = (avgTicks >= (long)(ticksTolFactor * targetTicks));
        bool timeout = (millis() - t0) > TIMEOUT_MS;

        if (reached || timeout)

            break;
    }

    brake();
}

void Robot::moveStraight(float speed)
{
    int error = MotorR.getTicks() - MotorL.getTicks();
    int correction = (int)_straightLinePID.compute((float)error);
    MotorR.setSpeed(speed + correction);
    MotorL.setSpeed(speed - correction);
}

void Robot::followRamp(float speed)
{

    int tickR = MotorR.getTicks();
    int tickL = MotorL.getTicks();
    int error = tickR - tickL;

    // MotorR.resetTicks();
    // MotorL.resetTicks();

    // if (tickR < 5 && tickL < 5)
    // {
    //     speed -= 10;
    // }

    int correction = (int)_straightLinePID.compute((float)error);
    MotorR.setSpeed(constrain(speed + correction, -60, 0));
    MotorL.setSpeed(constrain(speed - correction, -60, 0));
}

void Robot::moveStraight()
{
    moveStraight(_speed);
}

void Robot::moveStraightGyro(int initYaw, int speed){
    int err = initYaw - imu.getYaw();
    int correction = (int)_straightLineGyroPID.compute(err);
    MotorR.setSpeed(speed - correction);
    MotorL.setSpeed(speed + correction);
}

void Robot::followLine(int speed)
{
    int error = ir.weightedSum();
    int correction = (int)_lineFollowerPID.compute((float)error);
    // Serial.print(error);
    // Serial.print(",");
    // Serial.println(correction);
    MotorR.setSpeed(speed - correction);
    MotorL.setSpeed(speed + correction);
}

void Robot::followSingleWall()
{
    int tof = leftTof.readRange();
    int tof2 = leftTof2.readRange();
    float error = tof - tof2;
    float distErr = ((float)tof + (float)tof2) / 2.0;

    int lineCorrection = (int)_singleWallFollowerPID.compute(error);
    int distanceCorrection = (int)_singleWallDistancePID.compute(distErr);
    // Serial.println(String(error)+","+String(distErr)+","+String(lineCorrection)+","+String(distanceCorrection));
    // int distanceCorrection = 0;
    MotorR.setSpeed(_speed + lineCorrection + distanceCorrection);
    MotorL.setSpeed(_speed - lineCorrection - distanceCorrection);
}

void Robot::followDoubleWall()
{
    int error = leftTof.readRange();
    int correction = (int)_doubleWallFollowerPID.compute((float)error);
    MotorR.setSpeed(_speed - correction);
    MotorL.setSpeed(_speed + correction);
}

void Robot::goTillTicks(long targetTicks)
{
    MotorL.resetTicks();
    MotorR.resetTicks();

    while (labs(MotorL.getTicks() + MotorR.getTicks()) / 2 < labs(targetTicks))
    {
        if (targetTicks>0)
        {
            Robot::moveStraight();
        }else{
            Robot::moveStraight(-20);
        }
        
        delay(1);
    }
    brake();
}

void Robot::goTillCM(float cm)
{
    long targetTicks = lround((cm / (3.14159 * MotorL.getWheelDiameter())) * MotorL.getTicksPerRev());
    Robot::goTillTicks(targetTicks);
}

void Robot::calibrateIR()
{
    // int angles[5] = {45, -90, 90, -90, 45};
    for (int i = 0; i < 200; i++)
    {
        // Robot::turn(angles[i], 10, [](Robot *r)
        //             { r->ir.updateSensors(); });
        ir.updateSensors();
        delay(5);
    }
    ir.calibrate();
}

void Robot::setInterruptButton(pushbutton &btn)
{
    _interruptButton = &btn;
}

bool Robot::isInterrupted()
{
    if (_interruptButton && _interruptButton->stateChanged() == 1)
    {
        Serial.println("Interrupt Button Pressed");
        stop();
        return true;
    }
    return false;
}

void Robot::setLineFollowerPID(float kp, float ki, float kd)
{
    _lineFollowerPID.updatePID(kp, ki, kd);
}

void Robot::setSingleWallFollowerPID(float kp, float ki, float kd)
{
    _singleWallFollowerPID.updatePID(kp, ki, kd);
}
void Robot::setDoubleWallFollowerPID(float kp, float ki, float kd)
{
    _singleWallFollowerPID.updatePID(kp, ki, kd);
}

void Robot::setStraightLinePID(float kp, float ki, float kd)
{
    _straightLinePID.updatePID(kp, ki, kd);
}

bool Robot::detectLeftBox()
{
    return leftTof.readRange() < 160;
}
bool Robot::detectRightBox()
{
    return rightTof.readRange() < 160;
}
bool Robot::detectFrontBox()
{
    return frontTof.readRange() < 200;
}
bool Robot::detectObstacle()
{
    return frontTof.readRange() < 200 && frontTopTof.readRange() < 200;
}

void Robot::IRDebug()
{
    bool dig[8];
    ir.readDigital(dig);
    for (int i = 0; i < 8; i++)
    {
        Serial.print(dig[i]);
        Serial.print(",");
    }
    Serial.println();
}

void Robot::goCell(int8_t cells)
{
    int8_t count = 0;
    bool triggered = false;
    while (!junction.isLine())
    {
        moveStraightGyro(30);
    }
    
    while (count < cells)
    {
        if(junction.isLine()){
            followLine(30);
            triggered = false;
        }
        else{
            if (!triggered){
                count++;
                triggered = true;
            }
            moveStraightGyro(30);
        }
    }
    brake();
    
}

ColorName Robot::detectColor()
{
    goTillCM(5);
    const int NUM_READS = 5;
    int counts[COLOR_WHITE + 1] = {0};  // indexes 0..5 
    for (int i = 0; i < NUM_READS; i++)
    {
        ColorName c = boxColorSensor.getColor();         
        if (c >= COLOR_UNKNOWN && c <= COLOR_WHITE)
        counts[(int)c]++;
        
        delay(100);  // 100 ms between samples
    } 
    // Find the enum value with the highest count
    int bestIndex = COLOR_UNKNOWN;
    for (int i = COLOR_UNKNOWN + 1; i <= COLOR_WHITE; i++)
    {
        if (counts[i] > counts[bestIndex])
            bestIndex = i;
    }

    goTillCM(-5);
    return (ColorName)bestIndex;
}