#include "TOF050C.h"
#include "I2CHandler.h" // for i2c_master_reg16_* helpers
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// VL6180X key registers (16-bit)
#define REG_SYSRANGE_START 0x0018
#define REG_SYSTEM_INTERRUPT_CLEAR 0x0015
#define REG_RESULT_INTERRUPT_STATUS_GPIO 0x004F
#define REG_RESULT_RANGE_VAL 0x0062

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
