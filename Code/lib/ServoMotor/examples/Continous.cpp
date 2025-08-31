#include <Arduino.h>
#include <ServoMotor.h>

ServoMotor wheel(6 /*pin*/, 1000, 2000, true);  // continuous

void setup() {
  Serial.begin(115200);
  wheel.init(0);          // stop
}

void loop() {
  wheel.setPercent(60);   // forward
  delay(1000);
  wheel.setPercent(0);    // stop
  delay(500);
  wheel.setPercent(-60);  // reverse
  delay(1000);
}
