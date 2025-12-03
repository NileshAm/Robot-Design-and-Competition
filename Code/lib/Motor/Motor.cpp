#include "Motor.h"
#include <Arduino.h>

// static registry definition
Motor *Motor::_reg[Motor::kMaxInts] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

Motor::Motor(uint8_t dir1, uint8_t dir2, uint8_t pwmF, uint8_t pwmB,
             uint8_t encA, uint8_t encB, uint16_t tpr, float wheelDiameter)
    : _dir1(dir1), _dir2(dir2), _pwmF(pwmF), _pwmB(pwmB), _encA(encA), _encB(encB),
      _ticks(0), _ticksPerRev(tpr), _lastMs(0), _lastTicks(0), _intNum(-1), _wheelDiameter(wheelDiameter){}

void Motor::init()
{
    pinMode(_dir1, OUTPUT);
    pinMode(_dir2, OUTPUT);
    pinMode(_pwmF, OUTPUT);
    pinMode(_pwmB, OUTPUT);
    digitalWrite(_dir1, LOW);
    digitalWrite(_dir2, LOW);
    analogWrite(_pwmF, 0);
    analogWrite(_pwmB, 0);

    pinMode(_encA, INPUT_PULLUP);
    pinMode(_encB, INPUT_PULLUP);

    _intNum = digitalPinToInterrupt(_encA);
    if (_intNum == NOT_AN_INTERRUPT)
    {
        Serial.println(F("[Motor] ERROR: encA is not an interrupt-capable pin (use 2,3,18,19,20,21 on Mega)."));
        while (1)
        {
            delay(1000);
        }
    }

    // register this instance for its interrupt line
    if (_intNum >= 0 && _intNum < (int)kMaxInts)
    {
        _reg[_intNum] = this;
        switch (_intNum)
        {
        case 0:
            attachInterrupt(0, _isr0, CHANGE);
            break; // pin 2
        case 1:
            attachInterrupt(1, _isr1, CHANGE);
            break; // pin 3
        case 2:
            attachInterrupt(2, _isr2, CHANGE);
            break; // pin 21
        case 3:
            attachInterrupt(3, _isr3, CHANGE);
            break; // pin 20
        case 4:
            attachInterrupt(4, _isr4, CHANGE);
            break; // pin 19
        case 5:
            attachInterrupt(5, _isr5, CHANGE);
            break; // pin 18
        default:
            break;
        }
    }

    _lastMs = millis();
    _lastTicks = 0;
}
void Motor::brake()
{
    analogWrite(_pwmF, 0);
    analogWrite(_pwmB, 0);
    digitalWrite(_dir1, HIGH);
    digitalWrite(_dir2, HIGH);
}
void Motor::setSpeed(float pct)
{
    pct = constrain(pct, -100, 100);
    uint8_t duty = map(abs(pct), 0, 100, 0, 255);

    digitalWrite(_dir1, HIGH);
    digitalWrite(_dir2, HIGH);
    if (pct > 0)
    {
        analogWrite(_pwmB, 0);
        analogWrite(_pwmF, duty);
    }
    else if (pct < 0)
    {
        analogWrite(_pwmF, 0);
        analogWrite(_pwmB, duty);
    }
    else
    {
        digitalWrite(_dir1, LOW);
        digitalWrite(_dir2, LOW);
        analogWrite(_pwmF, 0);
        analogWrite(_pwmB, 0);
    }
}

long Motor::getTicks()
{
    noInterrupts();
    long t = _ticks;
    interrupts();
    return t;
}

void Motor::resetTicks()
{
    noInterrupts();
    _ticks = 0;
    interrupts();
    _lastMs = millis();
    _lastTicks = 0;
}

float Motor::getRPM()
{
    unsigned long now = millis();
    long tNow;

    noInterrupts();
    tNow = _ticks;
    interrupts();

    unsigned long dt = now - _lastMs;
    if (dt == 0 || _ticksPerRev == 0)
        return 0.0f;

    long dTicks = tNow - _lastTicks;
    float revs = (float)dTicks / (float)_ticksPerRev;
    float rpm = revs * (60000.0f / (float)dt);

    _lastMs = now;
    _lastTicks = tNow;
    return rpm;
}

float Motor::getWheelDiameter()
{
    return _wheelDiameter;
}
float Motor::getTicksPerRev()
{
    return _ticksPerRev;
}

void Motor::goTillTicks(long targetTicks, float speed)
{
    long startTicks = getTicks();
    long endTicks = startTicks + targetTicks;
    if (targetTicks > 0)
    {
        setSpeed(abs(speed));
        while (getTicks() < endTicks)
        {
            // wait
            delay(1);
        }
    }
    else if (targetTicks < 0)
    {
        setSpeed(-abs(speed));
        while (getTicks() > endTicks)
        {
            // wait
            delay(1);
        }
    }
    setSpeed(0);
}

void Motor::goTillCM(float cm, float speed)
{
    long targetTicks = getTicksPerDistance(cm);
    goTillTicks(targetTicks, speed);
}

long Motor::getTicksPerDistance(float cm)
{
    if (_ticksPerRev == 0 || _wheelDiameter <= 0.0f)
        return 0;

    float revs = cm / (3.14159f * _wheelDiameter);
    long targetTicks = (long)(revs * (float)_ticksPerRev);
    return targetTicks;
}

inline void Motor::handleEncoder()
{
    // Simple quadrature: read both channels on any CHANGE of A-line
    bool a = digitalRead(_encA);
    bool b = digitalRead(_encB);

    // Adjust the rule below if your wiring reverses direction:
    // If A == B, count +1; else -1
    if (a == b)
        _ticks++;
    else
        _ticks--;
}

// ----- static dispatch + ISRs -----
inline void Motor::_dispatch(uint8_t intNum)
{
    Motor *m = (intNum < kMaxInts) ? _reg[intNum] : nullptr;
    if (m)
        m->handleEncoder();
}

void Motor::_isr0() { _dispatch(0); }
void Motor::_isr1() { _dispatch(1); }
void Motor::_isr2() { _dispatch(2); }
void Motor::_isr3() { _dispatch(3); }
void Motor::_isr4() { _dispatch(4); }
void Motor::_isr5() { _dispatch(5); }
