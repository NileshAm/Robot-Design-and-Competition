#include <Arduino.h>
#include <ServoMotor.h>

ServoMotor arm(A0 /*pin*/, 1000, 2000, false);  // positional

void setup() {
  Serial.begin(115200);
  arm.init(90);  // go to center
}

void loop() {
  arm.writeAngle(30);  delay(700);
  arm.writeAngle(150); delay(700);
  arm.setPercent(0);   delay(700);   // -> 90°
}
