#include "PWMConfig.h"

static bool timer_initialized[LEDC_TIMER_MAX][LEDC_SPEED_MODE_MAX] = {false};

esp_err_t setPWM(PWM *pwm, int duty)
{
    uint32_t max_duty = (1 << pwm->timer_config.duty_resolution) - 1;

    ESP_RETURN_ON_FALSE(duty <= max_duty, ESP_ERR_INVALID_ARG, "PWM", "PWM value %d exceeds maximum %" PRIu32, duty, max_duty);

    esp_err_t err = ledc_set_duty(pwm->timer_config.speed_mode,
                                  pwm->channel_config.channel,
                                  duty);
    ESP_RETURN_ON_ERROR(err, "PWM", "ledc_set_duty failed");

    ledc_update_duty(pwm->timer_config.speed_mode, pwm->channel_config.channel);

    return ESP_OK;
}

esp_err_t setPWMPercentage(PWM *pwm, float percentage)
{
    ESP_RETURN_ON_FALSE(percentage >= 0 && percentage <= 100,
                        ESP_ERR_INVALID_ARG, "PWM",
                        "Percentage %.2f must be between 0–100", percentage);

    uint32_t max_duty = (1 << pwm->timer_config.duty_resolution) - 1;
    uint32_t duty = (uint32_t)(max_duty * (percentage / 100.0f));

    esp_err_t err = ledc_set_duty(pwm->timer_config.speed_mode,
                                  pwm->channel_config.channel,
                                  duty);
    ESP_RETURN_ON_ERROR(err, "PWM", "ledc_set_duty failed");

    ledc_update_duty(pwm->timer_config.speed_mode, pwm->channel_config.channel);

    return ESP_OK;
}

esp_err_t initPWM(PWM *pwm)
{
    ledc_timer_t tnum = pwm->timer_config.timer_num;
    ledc_mode_t mode = pwm->timer_config.speed_mode;

    // If timer hasn't been initialized, do it now
    if (!timer_initialized[tnum][mode])
    {
        esp_err_t err = ledc_timer_config(&pwm->timer_config);
        ESP_RETURN_ON_ERROR(err, "PWM", "Timer config failed");
        timer_initialized[tnum][mode] = true;
    }

    // Always configure the channel
    ledc_channel_config_t channel_config = {
        .speed_mode = pwm->timer_config.speed_mode,
        .channel = pwm->channel_config.channel,
        .timer_sel = pwm->timer_config.timer_num,
        .intr_type = pwm->channel_config.intr_type,
        .gpio_num = pwm->channel_config.gpio_num,
        .duty = pwm->channel_config.duty,
        .hpoint = pwm->channel_config.hpoint};
    esp_err_t err = ledc_channel_config(&channel_config);
    ESP_RETURN_ON_ERROR(err, "PWM", "Channel config failed");

    return ESP_OK;
}
