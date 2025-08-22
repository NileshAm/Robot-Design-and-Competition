#include "analog.h"
#include "esp_log.h"
#include "hal/adc_types.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"

/* Choose a non-deprecated default attenuation.
 * Override by defining ANALOG_DEFAULT_ATTEN before including analog.h. */
#ifndef ANALOG_DEFAULT_ATTEN
#define ANALOG_DEFAULT_ATTEN ADC_ATTEN_DB_12
#endif

/* One oneshot unit per ADC (created on first use) */
static adc_oneshot_unit_handle_t s_unit1, s_unit2;

static esp_err_t ensure_unit(adc_unit_t unit)
{
    if (unit == ADC_UNIT_1)
    {
        if (s_unit1)
            return ESP_OK;
        adc_oneshot_unit_init_cfg_t cfg = {.unit_id = ADC_UNIT_1};
        return adc_oneshot_new_unit(&cfg, &s_unit1);
    }
    else if (unit == ADC_UNIT_2)
    {
        if (s_unit2)
            return ESP_OK;
        adc_oneshot_unit_init_cfg_t cfg = {.unit_id = ADC_UNIT_2};
        return adc_oneshot_new_unit(&cfg, &s_unit2);
    }
    return ESP_ERR_INVALID_ARG;
}

esp_err_t analog_init(gpio_num_t pin, adc_width_t width)
{
    adc_unit_t unit;
    adc_channel_t ch;
    esp_err_t e = adc_oneshot_io_to_channel(pin, &unit, &ch);
    if (e != ESP_OK)
        return e;

    e = ensure_unit(unit);
    if (e != ESP_OK)
        return e;

    adc_oneshot_unit_handle_t uh = (unit == ADC_UNIT_1) ? s_unit1 : s_unit2;
    adc_oneshot_chan_cfg_t cfg = {.bitwidth = width, .atten = ANALOG_DEFAULT_ATTEN};
    return adc_oneshot_config_channel(uh, ch, &cfg);
}

int analog_read(gpio_num_t pin)
{
    adc_unit_t unit;
    adc_channel_t ch;
    if (adc_oneshot_io_to_channel(pin, &unit, &ch) != ESP_OK)
        return -1;
    if (ensure_unit(unit) != ESP_OK)
        return -2;

    /* If user forgot analog_init(), do a quick sane default config */
    adc_oneshot_unit_handle_t uh = (unit == ADC_UNIT_1) ? s_unit1 : s_unit2;
    adc_oneshot_chan_cfg_t cfg = {.bitwidth = ADC_BITWIDTH_12, .atten = ANALOG_DEFAULT_ATTEN};
    (void)adc_oneshot_config_channel(uh, ch, &cfg);

    int raw = 0;
    return (adc_oneshot_read(uh, ch, &raw) == ESP_OK) ? raw : -3;
}
