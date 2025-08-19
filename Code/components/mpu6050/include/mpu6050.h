#pragma once
#include <stdint.h>
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MPU6050_ADDR_7BIT 0x68  // 0x69 if AD0 is high

esp_err_t mpu6050_init_device(i2c_master_dev_handle_t dev);
esp_err_t mpu6050_read_accel_gyro_temp(i2c_master_dev_handle_t dev,
                                       float *ax_g, float *ay_g, float *az_g,
                                       float *gx_dps, float *gy_dps, float *gz_dps,
                                       float *temp_c);

#ifdef __cplusplus
}
#endif
