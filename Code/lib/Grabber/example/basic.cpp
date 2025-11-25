#include "ServoMotor.h"
#include "CurrentSensor.h"
#include "Grabber.h"

ServoMotor grabServo(9);       // grabber servo pin
ServoMotor liftServo(10);      // lifter servo pin
CurrentSensor currentSensor(A0, 0.185, 2.5, 0.1);  // pin, sens, offset, spikeThreshold

Grabber grabber(grabServo, liftServo, currentSensor);

void setup() {
    Serial.begin(9600);
    grabServo.attach();
    liftServo.attach();
}

void loop() {
    grabber.grab();
    grabber.lift();
    delay(1000);
    grabber.reset();
    grabber.release();
    delay(3000);
}
