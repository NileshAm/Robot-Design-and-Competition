#include "I2CHandler.h"
#include "TOF050C.h"
#include "driver/gpio.h"

#define TOF050C_ADDR_7BIT 0x29

// constants for setting up multiple TOF050C devices
#define TOF1_CE GPIO_NUM_4 // pin for XSHUT of first TOF050C
#define TOF2_CE GPIO_NUM_5 // pin for XSHUT of second TOF050C
#define TOF1_ADDR 0x2A     // new I2C address for first TOF050C
#define TOF2_ADDR 0x2B     // new I2C address for second TOF050C

void app_main(void)
{
    i2c_master_bus_handle_t bus;
    i2c_master_dev_handle_t tof_dev;

    // Use your existing init functions
    i2c_master_init_bus(GPIO_NUM_22, GPIO_NUM_22, &bus);
    i2c_master_init_handle(&bus, &tof_dev, TOF050C_ADDR_7BIT);

    // Init devices
    tof050c_init_device(tof_dev);

    // Setting up multiple TOF050C devices
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << TOF1_CE) | (1ULL << TOF2_CE),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io);
    gpio_set_level(TOF1_CE, 0);
    gpio_set_level(TOF2_CE, 0);

    i2c_master_dev_handle_t tof1, tof2;
    tof050c_boot_and_set_address(bus, TOF1_CE, TOF1_ADDR, &tof1);
    tof050c_boot_and_set_address(bus, TOF2_CE, TOF2_ADDR, &tof2);

    // Quick demo reads (put into tasks if you like)
    while (1)
    {
        uint16_t mm = 0;
        if (tof050c_read_range_mm(tof_dev, &mm) == ESP_OK)
        {
            printf("TOF050C: %u mm\n", mm);
        }
        if (tof050c_read_range_mm(tof1, &mm) == ESP_OK)
        {
            printf("TOF050C: %u mm\n", mm);
        }
        if (tof050c_read_range_mm(tof2, &mm) == ESP_OK)
        {
            printf("TOF050C: %u mm\n", mm);
        }
    }
}