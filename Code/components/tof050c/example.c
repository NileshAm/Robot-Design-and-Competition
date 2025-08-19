#include "I2CHandler.h"
#include "TOF050C.h"

#define TOF050C_ADDR_7BIT 0x29

void app_main(void)
{
    i2c_master_bus_handle_t bus;
    i2c_master_dev_handle_t tof_dev;

    // Use your existing init functions
    i2c_master_init_bus(GPIO_NUM_22, GPIO_NUM_22, &bus);
    i2c_master_init_handle(&bus, &tof_dev, TOF050C_ADDR_7BIT);

    // Init devices
    tof050c_init_device(tof_dev);
    
    // Quick demo reads (put into tasks if you like)
    while (1) {
        uint16_t mm = 0;
        if (tof050c_read_range_mm(tof_dev, &mm) == ESP_OK) {
            printf("TOF050C: %u mm\n", mm);
        }
    }
}