#include "TOF050C.h"
#include "I2CHandler.h" // for i2c_master_reg16_* helpers
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// VL6180X key registers (16-bit)
#define REG_SYSRANGE_START 0x0018
#define REG_SYSTEM_INTERRUPT_CLEAR 0x0015
#define REG_RESULT_INTERRUPT_STATUS_GPIO 0x004F
#define REG_RESULT_RANGE_VAL 0x0062

#define TOF050C_DEFAULT_ADDR 0x29           // VL6180X default
#define REG_I2C_SLAVE_DEVICE_ADDRESS 0x0212 // 16-bit register

esp_err_t tof050c_init_device(i2c_master_dev_handle_t dev, uint8_t offset)
{
    static const struct
    {
        uint16_t reg;
        uint8_t val;
    } sr[] = {
        {0x0207, 0x01},
        {0x0208, 0x01},
        {0x0096, 0x00},
        {0x0097, 0xFD},
        {0x00E3, 0x00},
        {0x00E4, 0x04},
        {0x00E5, 0x02},
        {0x00E6, 0x01},
        {0x00E7, 0x03},
        {0x00F5, 0x02},
        {0x00D9, 0x05},
        {0x00DB, 0xCE},
        {0x00DC, 0x03},
        {0x00DD, 0xF8},
        {0x009F, 0x00},
        {0x00A3, 0x3C},
        {0x00B7, 0x00},
        {0x00BB, 0x3C},
        {0x00B2, 0x09},
        {0x00CA, 0x09},
        {0x0198, 0x01},
        {0x01B0, 0x17},
        {0x01AD, 0x00},
        {0x00FF, 0x05},
        {0x0100, 0x05},
        {0x0199, 0x05},
        {0x01A6, 0x1B},
        {0x01AC, 0x3E},
        {0x01A7, 0x1F},
        {0x0030, 0x00},
    };
    // Max convergence time = 63 ms (1 code = 1 ms)
    i2c_master_reg16_write8(dev, 0x01C, 63, 1000); // SYSRANGE__MAX_CONVERGENCE_TIME

    // Readout averaging period ~ 2.1 ms instead of ~4.3 ms default (48)
    i2c_master_reg16_write8(dev, 0x10A, 12, 1000); // READOUT__AVERAGING_SAMPLE_PERIOD

    // Disable range checks (ECE / Range Ignore / SNR) to keep it simple
    i2c_master_reg16_write8(dev, 0x02D, 0x00, 1000); // SYSRANGE__RANGE_CHECK_ENABLES

    for (size_t i = 0; i < sizeof sr / sizeof sr[0]; ++i)
    {
        esp_err_t e = i2c_master_reg16_write8(dev, sr[i].reg, sr[i].val, 1000);
        if (e != ESP_OK)
            return e;
    }

    // set the offset register to a known value
    i2c_master_reg16_write8(dev, 0x0024, (uint8_t)offset, 1000);

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

    uint8_t status = 0;
    i2c_master_reg16_read(dev, 0x004D, &status, 1, 1000);
    uint8_t code = (status >> 4) & 0x0F;

    ESP_LOGD("TOF050C", "Range: %u mm, Status: %u", (uint16_t)mm, (uint16_t)code);

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
