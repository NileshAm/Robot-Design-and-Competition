#pragma once
#include <stdint.h>
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t tof050c_init_device(i2c_master_dev_handle_t dev);
esp_err_t tof050c_read_range_mm(i2c_master_dev_handle_t dev, uint16_t *out_mm);

#ifdef __cplusplus
}
#endif
