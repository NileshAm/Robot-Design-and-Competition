#include <stdlib.h>
#include "motor.h"

struct motor_t {
    encoder_pcnt_handle_t enc;
    float cpr;
    TickType_t last;
    bool primed;
};

esp_err_t motor_create(const motor_config_t *cfg, motor_handle_t *out)
{
    if (!cfg || !cfg->encoder || cfg->cpr_output <= 0 || !out) return ESP_ERR_INVALID_ARG;
    motor_handle_t m = calloc(1, sizeof(*m));
    if (!m) return ESP_ERR_NO_MEM;
    m->enc = cfg->encoder;
    m->cpr = cfg->cpr_output;   // 825.0f for your motor (11*75*1)
    m->last = xTaskGetTickCount();
    m->primed = false;
    *out = m;
    return ESP_OK;
}

esp_err_t motor_get_tick_count_chA(motor_handle_t m, int64_t *ticks)
{
    if (!m || !ticks) return ESP_ERR_INVALID_ARG;
    return encoder_pcnt_get_total_chA(m->enc, ticks);
}

esp_err_t motor_get_tick_count_chB(motor_handle_t m, int64_t *ticks)
{
    if (!m || !ticks) return ESP_ERR_INVALID_ARG;
    return encoder_pcnt_get_total_chB(m->enc, ticks);
}

esp_err_t motor_get_speed_rpm(motor_handle_t m, float *rpm)
{
    if (!m || !rpm) return ESP_ERR_INVALID_ARG;

    TickType_t now = xTaskGetTickCount();
    TickType_t dt = now - m->last;
    m->last = now;

    int d = 0;
    ESP_ERROR_CHECK(encoder_pcnt_get_and_clear_delta_chA(m->enc, &d)); // A rising only

    if (!m->primed || dt == 0) { m->primed = true; *rpm = 0.0f; return ESP_OK; }

    float dt_ms = (float)dt * (float)portTICK_PERIOD_MS;
    float revs = d / m->cpr;
    *rpm = (revs * 60000.0f) / dt_ms;
    return ESP_OK;
}
