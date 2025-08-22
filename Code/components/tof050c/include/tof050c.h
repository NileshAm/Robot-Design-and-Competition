#pragma once
#include <stdint.h>
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t tof050c_init_device(i2c_master_dev_handle_t dev, uint8_t offset);
    esp_err_t tof050c_read_range_mm(i2c_master_dev_handle_t dev, uint16_t *out_mm);

    // Boot a single TOF050C (VL6180X) via its XSHUT pin, set a new 7-bit I2C address,
    // and return a device handle bound to that new address.
    esp_err_t tof050c_boot_and_set_address(i2c_master_bus_handle_t bus,
                                           gpio_num_t xshut_gpio,
                                           uint8_t new_addr,
                                           i2c_master_dev_handle_t *out_dev);

#ifdef __cplusplus
}
#endif
