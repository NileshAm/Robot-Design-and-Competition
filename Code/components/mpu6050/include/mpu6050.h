#pragma once
#include <stdint.h>
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t mpu6050_init_device(i2c_master_dev_handle_t dev);
esp_err_t mpu6050_read_accel_gyro_temp(i2c_master_dev_handle_t dev,
                                       float *ax_g, float *ay_g, float *az_g,
                                       float *gx_dps, float *gy_dps, float *gz_dps,
                                       float *temp_c);

// Raw (register) units
esp_err_t mpu6050_read_accel_raw(i2c_master_dev_handle_t dev, int16_t *ax, int16_t *ay, int16_t *az);
esp_err_t mpu6050_read_gyro_raw(i2c_master_dev_handle_t dev, int16_t *gx, int16_t *gy, int16_t *gz);
esp_err_t mpu6050_read_temp_raw(i2c_master_dev_handle_t dev, int16_t *t_raw);

// Scaled to physical units
esp_err_t mpu6050_read_accel_g(i2c_master_dev_handle_t dev, float *ax_g, float *ay_g, float *az_g);
esp_err_t mpu6050_read_gyro_dps(i2c_master_dev_handle_t dev, float *gx_dps, float *gy_dps, float *gz_dps);
esp_err_t mpu6050_read_temp_c(i2c_master_dev_handle_t dev, float *temp_c);

#ifdef __cplusplus
}
#endif
