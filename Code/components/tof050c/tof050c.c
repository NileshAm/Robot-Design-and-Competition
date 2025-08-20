#include "TOF050C.h"
#include "I2CHandler.h" // for i2c_master_reg16_* helpers
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// VL6180X key registers (16-bit)
#define REG_SYSRANGE_START 0x0018
#define REG_SYSTEM_INTERRUPT_CLEAR 0x0015
#define REG_RESULT_INTERRUPT_STATUS_GPIO 0x004F
#define REG_RESULT_RANGE_VAL 0x0062

#define TOF050C_DEFAULT_ADDR 0x29           // VL6180X default
#define REG_I2C_SLAVE_DEVICE_ADDRESS 0x0212 // 16-bit register

esp_err_t tof050c_init_device(i2c_master_dev_handle_t dev)
{
    // Minimal init (many modules ship preconfigured).
    // Add full ST "standard settings" here if needed.
    (void)dev;
    return ESP_OK;
}

esp_err_t tof050c_read_range_mm(i2c_master_dev_handle_t dev, uint16_t *out_mm)
{
    esp_err_t err = i2c_master_reg16_write8(dev, REG_SYSRANGE_START, 0x01, 1000);
    if (err != ESP_OK)
        return err;

    uint8_t istat = 0;
    TickType_t t0 = xTaskGetTickCount();
    do
    {
        err = i2c_master_reg16_read(dev, REG_RESULT_INTERRUPT_STATUS_GPIO, &istat, 1, 1000);
        if (err != ESP_OK)
            return err;
        if (istat & 0x04)
            break; // new sample ready
        vTaskDelay(pdMS_TO_TICKS(2));
    } while ((xTaskGetTickCount() - t0) < pdMS_TO_TICKS(100));

    uint8_t mm = 0;
    err = i2c_master_reg16_read(dev, REG_RESULT_RANGE_VAL, &mm, 1, 1000);
    if (err != ESP_OK)
        return err;

    (void)i2c_master_reg16_write8(dev, REG_SYSTEM_INTERRUPT_CLEAR, 0x07, 1000);

    if (out_mm)
        *out_mm = (uint16_t)mm;
    return ESP_OK;
}

esp_err_t tof050c_boot_and_set_address(i2c_master_bus_handle_t bus,
                                       gpio_num_t xshut_gpio,
                                       uint8_t new_addr,
                                       i2c_master_dev_handle_t *out_dev)
{
    if (!out_dev)
        return ESP_ERR_INVALID_ARG;

    // Reset -> boot the sensor so it answers at 0x29
    gpio_reset_pin(xshut_gpio);
    gpio_set_direction(xshut_gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(xshut_gpio, 0);
    vTaskDelay(pdMS_TO_TICKS(2));
    gpio_set_level(xshut_gpio, 1);
    vTaskDelay(pdMS_TO_TICKS(2));

    // Temporary handle at default 0x29 using your existing helper
    i2c_master_dev_handle_t tmp = NULL;
    i2c_master_init_handle(&bus, &tmp, TOF050C_DEFAULT_ADDR);

    // Write new 7-bit address into 0x0212 (big-endian reg index)
    uint8_t w[3] = {0x02, 0x12, new_addr};
    esp_err_t err = i2c_master_transmit(tmp, w, sizeof w, 1000);
    if (err != ESP_OK)
    {
        (void)i2c_master_bus_rm_device(tmp);
        return err;
    }

    // Remove handle bound to 0x29, then bind the new address
    (void)i2c_master_bus_rm_device(tmp);
    i2c_master_init_handle(&bus, out_dev, new_addr);

    return ESP_OK;
}
