#include <Arduino.h>
#include <UpdatePID.h>

UpdatePID pid(Serial);

void setup() {
    Serial.begin(9600);
    Serial.println("Waiting for PID data...");
}

void loop() {
    pid.update(); // constantly check for new data

    // Example: use the updated PID values
    float kp = pid.getKp();
    float ki = pid.getKi();
    float kd = pid.getKd();

    // You could use these in your PID controller here
    // ...
}
