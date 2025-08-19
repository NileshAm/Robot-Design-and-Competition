#include "I2CHandler.h"
#include "MPU6050.h"

#define MPU6050_ADDR_7BIT 0x68

void app_main(void)
{
    i2c_master_bus_handle_t bus;
    i2c_master_dev_handle_t mpu_dev;

    // Use your existing init functions
    i2c_master_init_bus(GPIO_NUM_22, GPIO_NUM_22, &bus);
    i2c_master_init_handle(&bus, &mpu_dev, MPU6050_ADDR_7BIT);

    // Init devices
    mpu6050_init_device(mpu_dev);
    
    // Quick demo reads (put into tasks if you like)
    while (1) {
        float ax, ay, az, gx, gy, gz, tc;
        if (mpu6050_read_accel_gyro_temp(mpu_dev, &ax, &ay, &az, &gx, &gy, &gz, &tc) == ESP_OK) {
            printf("MPU6050: a[g]=[%.3f %.3f %.3f] g  g[dps]=[%.2f %.2f %.2f]  T=%.2f C\n",
                   ax, ay, az, gx, gy, gz, tc);
        }
    }
}