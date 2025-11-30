#pragma once
#include <Arduino.h>

class Motor {
public:
  // dir1/dir2 = H-bridge direction pins, pwm = PWM pin
  // encA/encB = quadrature encoder pins (encA must be interrupt-capable)
  // ticksPerRev = encoder ticks per full shaft revolution
  Motor(uint8_t dir1, uint8_t dir2, uint8_t pwm,
        uint8_t encA, uint8_t encB, uint16_t ticksPerRev = 600, float wheelDiameter = 6.3);

  void init();                 // sets pinModes, checks/attaches interrupt
  void brake();
  void setSpeed(float pct);   // -100..100 (negative = reverse)
  long getTicks();             // thread-safe read
  float getWheelDiameter();
  float getTicksPerRev();
  void resetTicks();
  float getRPM();              // simple delta-based RPM

  void goTillTicks(long targetTicks, float speed);
  void goTillCM(float cm, float speed);

  void setTicksPerRev(uint16_t tpr) { _ticksPerRev = tpr; }

private:
  // pins
  uint8_t _dir1, _dir2, _pwm, _encA, _encB;

  // encoder state
  volatile long _ticks = 0;
  uint16_t _ticksPerRev;

  // rpm state
  unsigned long _lastMs = 0;
  long _lastTicks = 0;

  // which external interrupt number this instance uses (0..5 on Mega)
  int8_t _intNum = -1;
  
  // wheel diameter of the wheel attached to this motor (for distance calculations)
  float _wheelDiameter = 0; // in cm

  // called from ISR to update ticks
  inline void handleEncoder();

  // ---- minimal static registry/ISRs (one per interrupt line) ----
  static constexpr uint8_t kMaxInts = 6; // Mega has INT0..INT5
  static Motor* _reg[kMaxInts];

  static void _isr0();
  static void _isr1();
  static void _isr2();
  static void _isr3();
  static void _isr4();
  static void _isr5();

  static inline void _dispatch(uint8_t intNum);
};
