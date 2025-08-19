#pragma once

#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // I2C address scanning
    void check_i2c_address(void *arg);

    // Bus initialization
    void i2c_master_init_bus(gpio_num_t sda_pin, gpio_num_t scl_pin, i2c_master_bus_handle_t *bus_handle);
    void i2c_master_init_handle(i2c_master_bus_handle_t *bus_handle,
                                i2c_master_dev_handle_t *dev_handle,
                                uint8_t address);

    // 8-bit register devices (e.g., MPU6050)
    esp_err_t i2c_master_reg8_write8(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t val, int timeout_ms);
    esp_err_t i2c_master_reg8_write(i2c_master_dev_handle_t dev, uint8_t start_reg, const uint8_t *data, size_t len, int timeout_ms);
    esp_err_t i2c_master_reg8_read(i2c_master_dev_handle_t dev, uint8_t start_reg, uint8_t *buf, size_t len, int timeout_ms);

    // 16-bit register devices (e.g., TOF050C / VL6180X)
    esp_err_t i2c_master_reg16_write8(i2c_master_dev_handle_t dev, uint16_t reg, uint8_t val, int timeout_ms);
    esp_err_t i2c_master_reg16_read(i2c_master_dev_handle_t dev, uint16_t start_reg, uint8_t *buf, size_t len, int timeout_ms);

#ifdef __cplusplus
}
#endif
