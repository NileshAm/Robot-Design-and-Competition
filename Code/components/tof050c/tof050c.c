#include "TOF050C.h"
#include "I2CHandler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define REG_SYSRANGE_START 0x0018
#define REG_SYSTEM_INTERRUPT_CLEAR 0x0015
#define REG_RESULT_INTERRUPT_STATUS_GPIO 0x004F
#define REG_RESULT_RANGE_VAL 0x0062
#define REG_RESULT_RANGE_STATUS 0x004D
#define REG_PART_TO_PART_RANGE_OFFSET 0x0024

static esp_err_t write_defaults(i2c_master_dev_handle_t dev)
{
    static const struct
    {
        uint16_t r;
        uint8_t v;
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
        {0x001C, 63},
        {0x010A, 12},
    };
    for (size_t i = 0; i < sizeof sr / sizeof sr[0]; ++i)
    {
        esp_err_t e = i2c_master_reg16_write8(dev, sr[i].r, sr[i].v, 1000);
        if (e != ESP_OK)
            return e;
    }
    return ESP_OK;
}

esp_err_t tof_init(tof_dev_t *tof,
                   i2c_master_bus_handle_t bus,
                   gpio_num_t xshut_pin,
                   uint8_t addr7,
                   const char *tag,
                   int8_t offset_mm)
{
    if (!tof || !bus || addr7 == 0)
        return ESP_ERR_INVALID_ARG;

    // 1) Bring THIS sensor out of reset and keep it as OUTPUT (no internal pull-ups!)
    gpio_reset_pin(xshut_pin);
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << xshut_pin,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE};
    ESP_ERROR_CHECK(gpio_config(&io));
    ESP_ERROR_CHECK(gpio_set_level(xshut_pin, 1));
    vTaskDelay(pdMS_TO_TICKS(2));

    // 2) Device always boots at 0x29. Bind TEMP handle there first.
    const uint8_t DEF = 0x29;
    i2c_master_dev_handle_t tmp = NULL;
    i2c_master_init_handle(&bus, &tmp, DEF);

    // (Optional but helpful) Probe to fail fast if still not alive
    // ESP_ERROR_CHECK(i2c_master_probe(&bus, DEF, 1000));

    // 3) If a different address was requested, set it now, then rebind
    if (addr7 != DEF)
    {
        uint8_t w[3] = {0x02, 0x12, addr7}; // REG_I2C_SLAVE_DEVICE_ADDRESS (0x0212)
        ESP_ERROR_CHECK(i2c_master_transmit(tmp, w, sizeof w, 1000));
        ESP_ERROR_CHECK(i2c_master_bus_rm_device(tmp));
        tmp = NULL;
        // Rebind at new address
        i2c_master_init_handle(&bus, &tmp, addr7);
    }

    // 4) Write defaults and offset on the FINAL handle (tmp)
    ESP_ERROR_CHECK(write_defaults(tmp));
    ESP_ERROR_CHECK(i2c_master_reg16_write8(tmp, REG_PART_TO_PART_RANGE_OFFSET,
                                            (uint8_t)offset_mm, 1000));

    // 5) Store everything into tof_dev_t
    tof->bus_handler = bus;
    tof->dev_handler = tmp; // final handle
    tof->xshut_pin = xshut_pin;
    tof->tag = tag ? tag : "TOF";
    tof->offset_mm = offset_mm;
    tof->addr7 = (addr7 ? addr7 : DEF);

    ESP_LOGI(tof->tag, "TOF init OK: addr=0x%02X, XSHUT=%d, offset=%d mm",
             tof->addr7, tof->xshut_pin, (int)tof->offset_mm);
    return ESP_OK;
}

esp_err_t tof_read(tof_dev_t *tof, uint16_t *out_mm)
{
    if (!tof || !tof->dev_handler)
        return ESP_ERR_INVALID_ARG;

    esp_err_t err = i2c_master_reg16_write8(tof->dev_handler,
                                            REG_SYSRANGE_START, 0x01, 1000);
    if (err != ESP_OK)
        return err;

    uint8_t istat = 0;
    TickType_t t0 = xTaskGetTickCount();
    do
    {
        err = i2c_master_reg16_read(tof->dev_handler,
                                    REG_RESULT_INTERRUPT_STATUS_GPIO,
                                    &istat, 1, 1000);
        if (err != ESP_OK)
            return err;
        if (istat & 0x04)
            break;
        vTaskDelay(pdMS_TO_TICKS(2));
    } while ((xTaskGetTickCount() - t0) < pdMS_TO_TICKS(100));

    uint8_t mm8 = 0;
    err = i2c_master_reg16_read(tof->dev_handler,
                                REG_RESULT_RANGE_VAL, &mm8, 1, 1000);
    if (err != ESP_OK)
        return err;

    uint8_t status = 0;
    if (i2c_master_reg16_read(tof->dev_handler,
                              REG_RESULT_RANGE_STATUS, &status, 1, 1000) == ESP_OK)
    {
        status = (status >> 4) & 0x0F;
    }

    (void)i2c_master_reg16_write8(tof->dev_handler,
                                  REG_SYSTEM_INTERRUPT_CLEAR, 0x07, 1000);

    uint16_t mm = (uint16_t)mm8;
    if (out_mm)
        *out_mm = mm;

    ESP_LOGD(tof->tag, "read: %u mm (status=%u)", (unsigned)mm, (unsigned)status);
    return ESP_OK;
}

esp_err_t tof_xhut_config(const gpio_num_t *pins, size_t n)
{
    if (!pins || n == 0) return ESP_ERR_INVALID_ARG;

    // Build a single bitmask for one-shot config()
    uint64_t mask = 0;
    for (size_t i = 0; i < n; ++i) {
        if (pins[i] < 0) continue;                // ignore invalid entries
        mask |= (1ULL << pins[i]);
    }
    if (mask == 0) return ESP_ERR_INVALID_ARG;

    gpio_config_t io = {
        .pin_bit_mask = mask,
        .mode = GPIO_MODE_OUTPUT,                 // push-pull output
        .pull_up_en = GPIO_PULLUP_DISABLE,        // no internal PU (avoid 3.3V on 2.8V rails)
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    esp_err_t err = gpio_config(&io);
    if (err != ESP_OK) return err;

    // Drive all selected pins LOW (hold sensors in reset)
    for (size_t i = 0; i < n; ++i) {
        if (pins[i] < 0) continue;
        gpio_set_level(pins[i], 0);
    }
    return ESP_OK;
}