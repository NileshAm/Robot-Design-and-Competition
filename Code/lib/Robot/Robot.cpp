#include "Robot.h"

Robot::Robot(Motor &Motor_R, Motor &Motor_L, IRArray &IR_Arr, Tof &frontTof, Tof &leftTof, Tof &leftTof2, Tof &frontTopTof, Tof &rightTof, ColorSensor &grabberSensor, ColorSensor &boxColorSensor, OLED &oled) : MotorR(Motor_R),MotorL(Motor_L),ir(IR_Arr),frontTof(frontTof),leftTof(leftTof),leftTof2(leftTof2),frontTopTof(frontTopTof),rightTof(rightTof),grabberSensor(grabberSensor),boxColorSensor(boxColorSensor),junction(IR_Arr),oled(oled),

_straightLinePID(0.04, 0, 0, 0),
_rampPID(1, 0, 0, 0),
// _lineFollowerPID(0.02,0,-0.6,1400),      40% pwm
// _lineFollowerPID(0.012,0.004,-0.025,1400),
_lineFollowerPID(0.02, -0.005, 0, 1400),
// _singleWallFollowerPID(2,0.15,0,0),
// _singleWallDistancePID(0.5,0,0.05,100),
_singleWallFollowerPID(1, 0.1, 0, 0),
_singleWallDistancePID(0.75, 0, 0, 120),
_doubleWallFollowerPID(0.25, 0.1, 0, 90)
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

void Robot::turn90()
{
    Robot::turn(90);
}

void Robot::turn(int angle)
{
    _turnCore(angle, /*useCb=*/false, 0, nullptr);
}

void Robot::turn(int angle, uint16_t cbEveryMs, TurnCallback cb)
{
    _turnCore(angle, /*useCb=*/true, cbEveryMs ? cbEveryMs : 1, cb);
}

void Robot::_turnCore(int angle, bool useCb, uint16_t cbEveryMs, TurnCallback cb)
{
    PID turnPID(0.1, 0, 0, 0);
    if (angle == 0)
        return;

    MotorL.resetTicks();
    MotorR.resetTicks();

    const long target = lround(fabsf(_ticksPerDegree * (float)angle));
    const int dir = (angle >= 0) ? 1 : -1;

    MotorL.setSpeed(_speed * dir);
    MotorR.setSpeed(-_speed * dir);

    unsigned long nextCb = millis() + cbEveryMs;
    const unsigned long TIMEOUT_MS = 8000;
    const unsigned long t0 = millis();

    while (labs(MotorL.getTicks()) < target || labs(MotorR.getTicks()) < target)
    {
        const unsigned long now = millis();

        if (useCb && (long)(now - nextCb) >= 0)
        {
            if (cb)
                cb(this); // e.g., [](Robot* r){ r->ir.updateSensors(); }
            nextCb += cbEveryMs;
        }

        long L = labs(MotorL.getTicks());
        long R = labs(MotorR.getTicks());

        int error = L - R;
        int correction = (int)turnPID.compute((float)error);

        // if (L >= target)
        //     MotorL.setSpeed(0);
        // if (R >= target)
        //     MotorR.setSpeed(0);

        // if ((L >= target && R >= target) || (now - t0 > TIMEOUT_MS))
        //     break;
        float speedFactor = 1.1;
        MotorR.setSpeed((-_speed * dir * speedFactor) - correction);
        MotorL.setSpeed((_speed * dir * speedFactor) + correction);
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

    while (fabs(MotorL.getTicks() + MotorR.getTicks()) / 2 < targetTicks)
    {
        Robot::moveStraight();
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
    return leftTof.readRange() < 150;
}
bool Robot::detectRightBox()
{
    return rightTof.readRange() < 150;
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
    ir.digitalRead(dig);
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

    enum State
    {
        ON_LINE,
        OFF_LINE_WAIT,
        COUNTED
    };
    State state = ON_LINE;

    unsigned long offLineStart = 0;
    const unsigned long debounceTime = 50; // 50ms noise filter

    while (count < cells)
    {
        bool onLine = junction.isLine();

        switch (state)
        {

        case ON_LINE:
            if (!onLine)
            {
                offLineStart = millis();
                state = OFF_LINE_WAIT;
            }
            followLine(20);
            break;

        case OFF_LINE_WAIT:
            // Still off-line and long enough to be a real junction?
            if (!onLine && millis() - offLineStart >= debounceTime)
            {
                count++;
                Serial.println(count);
                state = COUNTED;
            }
            // Noise: returned to line too fast → return to ON_LINE
            else if (onLine)
            {
                state = ON_LINE;
            }
            moveStraight(20);
            break;

        case COUNTED:
            // Stay in COUNTED until line is properly regained
            if (onLine)
            {
                state = ON_LINE;
            }
            moveStraight(20);
            break;
        }
    }
    brake();
}