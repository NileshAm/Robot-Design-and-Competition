#include <stdio.h>
#include "I2CHandler.h"
#include "esp_check.h"

void i2c_master_init_bus(gpio_num_t sda_pin, gpio_num_t scl_pin,  i2c_master_bus_handle_t *bus_handle)
{
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = sda_pin,
        .scl_io_num = scl_pin,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = { .enable_internal_pullup = true },
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, bus_handle));
}


void i2c_master_init_handle(i2c_master_bus_handle_t *bus_handle,
                            i2c_master_dev_handle_t *dev_handle,
                            uint8_t address)
{
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = address,
        .scl_speed_hz = 100000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(*bus_handle, &dev_config, dev_handle));
}

// check for I2C devices on the bus
void check_i2c_address(void *arg)
{
    i2c_master_bus_handle_t bus_handle = (i2c_master_bus_handle_t)arg;

    while (1) {
        for (uint8_t i = 0x03; i < 0x78; i++) {
            esp_err_t err = i2c_master_probe(bus_handle, i, 1000);
            if (err == ESP_OK) {
                printf("I2C device at: 0x%02X\n", i);
            }
        }
        printf("I2C Scanner Complete\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
