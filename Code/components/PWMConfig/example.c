#include "PWMConfig.h"
#include "driver/ledc.h"
#include "esp_log.h"

void app_main(void)
{
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,  // LEDC speed mode, low-speed mode is typically used for battery-powered devices
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_12_BIT, // resolution of the pwm signal
        .freq_hz = 10000,   // frequency of the pwm signal
        .clk_cfg = LEDC_AUTO_CLK
    };

    PWM pwm = {
        .timer_config = timer,
        .channel_config = {
            .channel = LEDC_CHANNEL_0,
            .intr_type = LEDC_INTR_DISABLE,
            .gpio_num = GPIO_NUM_18, // TODO: Change to your GPIO pin
            .duty = 0, // Initial duty cycle
            .hpoint = 0
        }
    }; 
    
    initPWM(&pwm);

    while (true)
    {
        setPWMPercentage(&pwm, 50.0f); // Set PWM to 50% duty cycle
        
        setPWM(&pwm, 2048); // Set PWM to a specific duty cycle (2048 for 50% in 12-bit resolution)
    }
}