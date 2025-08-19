#include "MPU6050.h"
#include "I2CHandler.h" // for i2c_master_reg8_* helpers
#include "esp_check.h"

// Registers
#define REG_PWR_MGMT_1 0x6B
#define REG_SMPLRT_DIV 0x19
#define REG_CONFIG 0x1A
#define REG_GYRO_CONFIG 0x1B
#define REG_ACCEL_CONFIG 0x1C
#define REG_ACCEL_XOUT_H 0x3B

// Scales (±2g, ±250 dps)
#define ACCEL_LSB_PER_G 16384.0f
#define GYRO_LSB_PER_DPS 131.0f

static inline int16_t be16(const uint8_t *p) { return (int16_t)((p[0] << 8) | p[1]); }

esp_err_t mpu6050_init_device(i2c_master_dev_handle_t dev)
{
    char TAG[] = "MPU6050";
    ESP_RETURN_ON_ERROR(i2c_master_reg8_write8(dev, REG_PWR_MGMT_1, 0x00, 1000), TAG, "PWR_MGMT_1");
    ESP_RETURN_ON_ERROR(i2c_master_reg8_write8(dev, REG_CONFIG, 0x03, 1000), TAG, "CONFIG");
    ESP_RETURN_ON_ERROR(i2c_master_reg8_write8(dev, REG_SMPLRT_DIV, 0x07, 1000), TAG, "SMPLRT_DIV");
    ESP_RETURN_ON_ERROR(i2c_master_reg8_write8(dev, REG_ACCEL_CONFIG, 0x00, 1000), TAG, "ACCEL_CONFIG");
    ESP_RETURN_ON_ERROR(i2c_master_reg8_write8(dev, REG_GYRO_CONFIG, 0x00, 1000), TAG, "GYRO_CONFIG");
    return ESP_OK;
}

esp_err_t mpu6050_read_accel_gyro_temp(i2c_master_dev_handle_t dev,
                                       float *ax_g, float *ay_g, float *az_g,
                                       float *gx_dps, float *gy_dps, float *gz_dps,
                                       float *temp_c)
{
    uint8_t raw[14];
    esp_err_t err = i2c_master_reg8_read(dev, REG_ACCEL_XOUT_H, raw, sizeof raw, 1000);
    if (err != ESP_OK)
        return err;

    int16_t ax = be16(&raw[0]);
    int16_t ay = be16(&raw[2]);
    int16_t az = be16(&raw[4]);
    int16_t t = be16(&raw[6]);
    int16_t gx = be16(&raw[8]);
    int16_t gy = be16(&raw[10]);
    int16_t gz = be16(&raw[12]);

    if (ax_g)
        *ax_g = ax / ACCEL_LSB_PER_G;
    if (ay_g)
        *ay_g = ay / ACCEL_LSB_PER_G;
    if (az_g)
        *az_g = az / ACCEL_LSB_PER_G;

    if (gx_dps)
        *gx_dps = gx / GYRO_LSB_PER_DPS;
    if (gy_dps)
        *gy_dps = gy / GYRO_LSB_PER_DPS;
    if (gz_dps)
        *gz_dps = gz / GYRO_LSB_PER_DPS;

    if (temp_c)
        *temp_c = (t / 340.0f) + 36.53f;

    return ESP_OK;
}
