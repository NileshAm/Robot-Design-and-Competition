#pragma once
#include <stdint.h>
#include "driver/gpio.h"
#include "esp_err.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_oneshot.h"

static const adc_bitwidth_t ANALOG_WIDTH_9 = ADC_BITWIDTH_9;
static const adc_bitwidth_t ANALOG_WIDTH_10 = ADC_BITWIDTH_10;
static const adc_bitwidth_t ANALOG_WIDTH_11 = ADC_BITWIDTH_11;
static const adc_bitwidth_t ANALOG_WIDTH_12 = ADC_BITWIDTH_12;

// Export attenuation constants
static const adc_atten_t ANALOG_ATTEN_0DB = ADC_ATTEN_DB_0;
static const adc_atten_t ANALOG_ATTEN_2DB = ADC_ATTEN_DB_2_5; // 2.5 dB in new IDF
static const adc_atten_t ANALOG_ATTEN_6DB = ADC_ATTEN_DB_6;
static const adc_atten_t ANALOG_ATTEN_12DB = ADC_ATTEN_DB_12; // ~3.3 V full range

/* Public API */
typedef adc_bitwidth_t adc_width_t;

/* Configure an ADC channel (atten set via ANALOG_DEFAULT_ATTEN). */
esp_err_t analog_init(gpio_num_t pin, adc_width_t width);

/* Read raw ADC sample. Returns negative value on error. */
int analog_read(gpio_num_t pin);
