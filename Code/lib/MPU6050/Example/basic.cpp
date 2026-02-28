#include "MPU6050.h"

MPU6050 imu;

void setup() {
    Serial.begin(115200);
    imu.begin();
}

void loop() {
    // One-shot update, then reuse without extra I2C
    imu.update();
    float roll  = imu.getRoll(false);
    float pitch = imu.getPitch(false);
    float yaw   = imu.getYaw(false);

    Serial.print("Roll: ");  Serial.print(roll);
    Serial.print(" Pitch: ");Serial.print(pitch);
    Serial.print(" Yaw: ");  Serial.println(yaw);

    delay(50);
}
