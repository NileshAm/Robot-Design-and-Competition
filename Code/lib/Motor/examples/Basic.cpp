#include <Arduino.h>
#include <Motor.h>

// Example pins (adjust for your setup)
// encA must be interrupt-capable: 2, 3, 18, 19, 20, 21 on Mega
// Interchange dir1 dir2 to reverse motor direction
Motor leftMotor(7, 8, 9,  2, 4, 600);   // dir1, dir2, pwm, encA, encB, ticks/rev
Motor rightMotor(22, 23, 10, 3, 5, 600);

void setup() {
  Serial.begin(115200);

  leftMotor.init();
  rightMotor.init();

  Serial.println("Motor library basic example");
}

void loop() {
  // Run motors
  leftMotor.setSpeed(50);    // forward 50%
  rightMotor.setSpeed(-50);  // backward 50%

  // Print stats every second
  static unsigned long last = 0;
  if (millis() - last > 1000) {
    last = millis();

    Serial.print("Left ticks="); Serial.print(leftMotor.getTicks());
    Serial.print(" rpm=");       Serial.print(leftMotor.getRPM(), 1);

    Serial.print(" | Right ticks="); Serial.print(rightMotor.getTicks());
    Serial.print(" rpm=");           Serial.println(rightMotor.getRPM(), 1);
  }
}
