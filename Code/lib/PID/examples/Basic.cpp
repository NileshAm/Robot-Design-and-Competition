#include <Arduino.h>
#include <PID.h>

PID speedPID(1.0f, 0.1f, 0.01f, 120.0f);  // Kp, Ki, Kd, setpoint

void setup() {
    Serial.begin(115200);
    // tweak individually anytime:
    speedPID.setKp(1.2f);
    speedPID.setKi(0.08f);
    speedPID.setKd(0.02f);
}

void loop() {
    float currentRPM = 95.0f;              // replace with sensor reading
    float out = speedPID.compute(currentRPM);

    // map 'out' to your motor command as needed
    // motor.setSpeed(constrain((int)out, -100, 100));

    Serial.print("PID out: "); Serial.println(out, 2);
    delay(50);
}
