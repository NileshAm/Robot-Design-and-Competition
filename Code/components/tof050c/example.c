#include "I2CHandler.h"
#include "TOF050C.h"
#include "Delay.h"
#include "esp_log.h"

#define TOF1_XSHUT GPIO_NUM_4 // Define the GPIO pin for the first TOF device's XSHUT
#define TOF2_XSHUT GPIO_NUM_5 // Define the GPIO pin for the second TOF device's XSHUT
#define TOF1_ADDR 0x2A        // Define the I2C address for the first TOF device
#define TOF2_ADDR 0x2B        // Define the I2C address for the second TOF device

void app_main(void)
{
    esp_log_level_set("TOF1", ESP_LOG_DEBUG);

    i2c_master_bus_handle_t bus;
    i2c_master_init_bus(GPIO_NUM_21, GPIO_NUM_22, &bus);

    // Configure XSHUT pins for multiple TOF devices
    const gpio_num_t xshuts[] = {GPIO_NUM_4, GPIO_NUM_5};
    tof_xhut_config(xshuts, sizeof(xshuts) / sizeof(xshuts[0]));

    // Init sensors (no external dev handles needed)
    tof_dev_t tof1, tof2;
    tof_init(&tof1, bus, TOF1_XSHUT, TOF1_ADDR, "TOF1", 30);
    tof_init(&tof2, bus, TOF2_XSHUT, TOF2_ADDR, "TOF2", 10);

    while (1)
    {
        uint16_t d1 = 0, d2 = 0;
        tof_read(&tof1, &d1);
        tof_read(&tof2, &d2);
        ESP_LOGI("TOF1", "Distance: %u mm", d1);
        ESP_LOGI("TOF2", "Distance: %u mm", d2);
    }
}
