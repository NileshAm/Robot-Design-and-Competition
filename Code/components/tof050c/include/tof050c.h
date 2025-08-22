#pragma once
#include <stdint.h>
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        i2c_master_bus_handle_t bus_handler;
        i2c_master_dev_handle_t dev_handler;
        gpio_num_t xshut_pin;
        const char *tag;
        int8_t offset_mm;
        uint8_t addr7;
    } tof_dev_t;

    /**
     * Initialize a TOF device:
     *  - Pull XSHUT HIGH
     *  - Create I²C device handle at given address
     *  - Write defaults and offset
     *  - Store everything inside tof_dev_t
     */
    esp_err_t tof_init(tof_dev_t *tof,
                       i2c_master_bus_handle_t bus,
                       gpio_num_t xshut_pin,
                       uint8_t addr7,
                       const char *tag,
                       int8_t offset_mm);

    /**
     * Read one distance sample and log result.
     * This function will store distance in millimeters in `out_mm`.
     * It will also log the distance and status given the logging level is `ESP_LOG_DEBUG`.
     */
    esp_err_t tof_read(tof_dev_t *tof, uint16_t *out_mm);

    /**
     * Configure multiple TOF devices by pulling their XSHUT pins low.
     * This is useful for initializing multiple TOF devices at once.
     */
    esp_err_t tof_xhut_config(const gpio_num_t *pins, size_t n);
#ifdef __cplusplus
}
#endif
