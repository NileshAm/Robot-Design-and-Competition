#include "I2CHandler.h"

void app_main(void)
{
    i2c_master_bus_handle_t bus_handler;
    i2c_master_init_bus(GPIO_NUM_22, GPIO_NUM_23, &bus_handler); // Chnage the SDA and SCL pins as needed

    xTaskCreate(check_i2c_address, "i2c_scan", 4096, (void *)bus_handler, 5, NULL);
}