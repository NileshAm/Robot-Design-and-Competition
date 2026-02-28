#ifndef CUSTOM_MPU6050_H
#define CUSTOM_MPU6050_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

class MPU6050
{
public:
    MPU6050();

    bool begin();
    void update();  // read sensor and update internal values + angles

    // Acceleration (m/s^2)
    float getAccX(bool now = true);
    float getAccY(bool now = true);
    float getAccZ(bool now = true);

    // Gyro (deg/s)
    float getGyroX(bool now = true);
    float getGyroY(bool now = true);
    float getGyroZ(bool now = true);

    // Angles (deg)
    float getRoll(bool now = true);   // rotation around X
    float getPitch(bool now = true);  // rotation around Y
    float getYaw(bool now = true);    // rotation around Z

    // Temperature (°C)
    float getTemperature(bool now = true);

private:
    Adafruit_MPU6050 mpu;

    sensors_event_t accEvent, gyroEvent, tempEvent;

    float roll, pitch, yaw;
    unsigned long lastUpdateMs;
};

#endif
