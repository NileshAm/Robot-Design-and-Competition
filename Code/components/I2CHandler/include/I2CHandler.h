#pragma once

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void i2c_master_init_bus(gpio_num_t sda_pin, gpio_num_t scl_pin, i2c_master_bus_handle_t *bus_handle);
    void i2c_master_init_handle(i2c_master_bus_handle_t *bus_handle,
                                i2c_master_dev_handle_t *dev_handle,
                                uint8_t address);
    void check_i2c_address(void *arg);

#ifdef __cplusplus
}
#endif
