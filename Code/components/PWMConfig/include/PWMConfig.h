#ifndef PWMCONFIG_H
#define PWMCONFIG_H

#include "driver/ledc.h"
#include "esp_check.h"

typedef struct {
    ledc_channel_t channel;
    ledc_intr_type_t intr_type;
    gpio_num_t gpio_num;
    uint32_t duty;
    int hpoint;
} PWM_channel_config;

typedef struct {
    ledc_timer_config_t timer_config;
    PWM_channel_config channel_config;

} PWM;

esp_err_t initPWM(PWM *pwm);
esp_err_t setPWM(PWM *pwm, int duty);
esp_err_t setPWMPercentage(PWM *pwm, float percentage);

#endif