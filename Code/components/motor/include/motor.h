#pragma once
#include <stdint.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "encoder_pcnt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct motor_t* motor_handle_t;

typedef struct {
    encoder_pcnt_handle_t encoder;
    float cpr_output;   // counts per output-shaft rev (e.g., 3300.0f)
} motor_config_t;

esp_err_t motor_create(const motor_config_t *cfg, motor_handle_t *out);

esp_err_t motor_get_tick_count_chA(motor_handle_t m, int64_t *ticks);
esp_err_t motor_get_tick_count_chB(motor_handle_t m, int64_t *ticks);

esp_err_t motor_get_speed_rpm(motor_handle_t m, float *rpm);  // call periodically
// esp_err_t motor_reset_ticks(motor_handle_t m);

#ifdef __cplusplus
}
#endif
