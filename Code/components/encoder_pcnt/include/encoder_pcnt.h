#pragma once
#include <stdint.h>
#include "driver/gpio.h"
#include "driver/pulse_cnt.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct encoder_pcnt_t* encoder_pcnt_handle_t;

// Create two independent rising-edge counters (A and B)
esp_err_t encoder_pcnt_create(gpio_num_t gpioA, gpio_num_t gpioB, encoder_pcnt_handle_t *out);

// Per-channel deltas (clears the HW counter of that channel)
esp_err_t encoder_pcnt_get_and_clear_delta_chA(encoder_pcnt_handle_t enc, int *delta);
esp_err_t encoder_pcnt_get_and_clear_delta_chB(encoder_pcnt_handle_t enc, int *delta);

// Per-channel totals (non-destructive, includes internal accumulator)
esp_err_t encoder_pcnt_get_total_chA(encoder_pcnt_handle_t enc, int64_t *total);
esp_err_t encoder_pcnt_get_total_chB(encoder_pcnt_handle_t enc, int64_t *total);

// Reset both channels’ totals and HW counters
esp_err_t encoder_pcnt_reset(encoder_pcnt_handle_t enc);

#ifdef __cplusplus
}
#endif
