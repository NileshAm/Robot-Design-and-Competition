#include <stdio.h>
#include "I2CHandler.h"
#include "esp_check.h"
#include <string.h>   // for memcpy

#ifndef I2C_MASTER_TIMEOUT_MS
#define I2C_MASTER_TIMEOUT_MS 1000
#endif

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

// -------- 8-bit register helpers --------
esp_err_t i2c_master_reg8_write8(i2c_master_dev_handle_t dev, uint8_t reg, uint8_t val, int timeout_ms)
{
    uint8_t w[2] = { reg, val };
    return i2c_master_transmit(dev, w, sizeof w, timeout_ms > 0 ? timeout_ms : I2C_MASTER_TIMEOUT_MS);
}

esp_err_t i2c_master_reg8_write(i2c_master_dev_handle_t dev, uint8_t start_reg, const uint8_t *data, size_t len, int timeout_ms)
{
    // small buffer fast-path
    if (len <= 32) {
        uint8_t tmp[1 + 32];
        tmp[0] = start_reg;
        memcpy(&tmp[1], data, len);
        return i2c_master_transmit(dev, tmp, 1 + len, timeout_ms > 0 ? timeout_ms : I2C_MASTER_TIMEOUT_MS);
    }
    // chunked writes
    esp_err_t err = ESP_OK;
    uint8_t reg = start_reg;
    while (len && err == ESP_OK) {
        size_t chunk = len > 32 ? 32 : len;
        uint8_t tmp[1 + 32];
        tmp[0] = reg;
        memcpy(&tmp[1], data, chunk);
        err = i2c_master_transmit(dev, tmp, 1 + chunk, timeout_ms > 0 ? timeout_ms : I2C_MASTER_TIMEOUT_MS);
        reg += chunk;
        data += chunk;
        len  -= chunk;
    }
    return err;
}

esp_err_t i2c_master_reg8_read(i2c_master_dev_handle_t dev, uint8_t start_reg, uint8_t *buf, size_t len, int timeout_ms)
{
    return i2c_master_transmit_receive(dev, &start_reg, 1, buf, len,
                                       timeout_ms > 0 ? timeout_ms : I2C_MASTER_TIMEOUT_MS);
}

// -------- 16-bit register helpers (big-endian register index) --------
esp_err_t i2c_master_reg16_write8(i2c_master_dev_handle_t dev, uint16_t reg, uint8_t val, int timeout_ms)
{
    uint8_t w[3] = { (uint8_t)(reg >> 8), (uint8_t)(reg & 0xFF), val };
    return i2c_master_transmit(dev, w, sizeof w, timeout_ms > 0 ? timeout_ms : I2C_MASTER_TIMEOUT_MS);
}

esp_err_t i2c_master_reg16_read(i2c_master_dev_handle_t dev, uint16_t start_reg, uint8_t *buf, size_t len, int timeout_ms)
{
    uint8_t reg_be[2] = { (uint8_t)(start_reg >> 8), (uint8_t)(start_reg & 0xFF) };
    return i2c_master_transmit_receive(dev, reg_be, 2, buf, len,
                                       timeout_ms > 0 ? timeout_ms : I2C_MASTER_TIMEOUT_MS);
}