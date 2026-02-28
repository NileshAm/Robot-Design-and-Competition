#include "MPU6050.h"

#define RAD_TO_DEG 57.2957795131f

MPU6050::MPU6050()
    : roll(0.0f),
      pitch(0.0f),
      yaw(0.0f),
      lastUpdateMs(0)
{
}

bool MPU6050::begin()
{
    Wire.begin();

    if (!mpu.begin()) {
        Serial.println("MPU6050 not found!");
        return false;
    }

    // Basic config – tune if you want
    mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    Serial.println("MPU6050 initialized.");
    lastUpdateMs = millis();
    return true;
}

void MPU6050::update()
{
    // Read all events in one shot
    mpu.getEvent(&accEvent, &gyroEvent, &tempEvent);

    // Compute dt
    unsigned long now = millis();
    float dt = (now - lastUpdateMs) / 1000.0f;
    if (dt <= 0.0f) dt = 0.001f;
    lastUpdateMs = now;

    // Acc angles (deg)
    float accX = accEvent.acceleration.x;
    float accY = accEvent.acceleration.y;
    float accZ = accEvent.acceleration.z;

    float accRoll  = atan2(accY, accZ) * RAD_TO_DEG;
    float accPitch = atan2(-accX, sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;

    // Gyro rates (deg/s)
    float gX = gyroEvent.gyro.x * RAD_TO_DEG;
    float gY = gyroEvent.gyro.y * RAD_TO_DEG;
    float gZ = gyroEvent.gyro.z * RAD_TO_DEG;

    // Integrate gyro to angles
    float gyroRollDelta  = gX * dt;
    float gyroPitchDelta = gY * dt;
    float gyroYawDelta   = gZ * dt;

    // Complementary filter
    const float alpha = 0.98f;  // gyro weight

    roll  = alpha * (roll  + gyroRollDelta)  + (1.0f - alpha) * accRoll;
    pitch = alpha * (pitch + gyroPitchDelta) + (1.0f - alpha) * accPitch;
    yaw  += gyroYawDelta;  // yaw from gyro only (will drift)
}

// --------- getters ---------

// Acc (m/s^2)
float MPU6050::getAccX(bool now)
{
    if (now) update();
    return accEvent.acceleration.x;
}

float MPU6050::getAccY(bool now)
{
    if (now) update();
    return accEvent.acceleration.y;
}

float MPU6050::getAccZ(bool now)
{
    if (now) update();
    return accEvent.acceleration.z;
}

// Gyro (deg/s)
float MPU6050::getGyroX(bool now)
{
    if (now) update();
    return gyroEvent.gyro.x * RAD_TO_DEG;
}

float MPU6050::getGyroY(bool now)
{
    if (now) update();
    return gyroEvent.gyro.y * RAD_TO_DEG;
}

float MPU6050::getGyroZ(bool now)
{
    if (now) update();
    return gyroEvent.gyro.z * RAD_TO_DEG;
}

// Angles (deg)
float MPU6050::getRoll(bool now)
{
    if (now) update();
    return roll;
}

float MPU6050::getPitch(bool now)
{
    if (now) update();
    return pitch;
}

float MPU6050::getYaw(bool now)
{
    if (now) update();
    return yaw;
}

// Temperature (°C)
float MPU6050::getTemperature(bool now)
{
    if (now) update();
    return tempEvent.temperature;
}
