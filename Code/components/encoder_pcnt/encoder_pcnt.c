#include <stdlib.h>
#include "encoder_pcnt.h"

struct encoder_pcnt_t {
    pcnt_unit_handle_t unit_a, unit_b;
    pcnt_channel_handle_t cha, chb;
    int64_t total_a, total_b;
};

static esp_err_t make_rising_counter(gpio_num_t gpio, pcnt_unit_handle_t *unit, pcnt_channel_handle_t *ch)
{
    pcnt_unit_config_t u = { .high_limit = 32767, .low_limit = -32768 };
    ESP_ERROR_CHECK(pcnt_new_unit(&u, unit));

    pcnt_chan_config_t cfg = {
        .edge_gpio_num  = gpio,
        .level_gpio_num = -1,     // not used
    };
    ESP_ERROR_CHECK(pcnt_new_channel(*unit, &cfg, ch));

    // Rising edge = INCREASE, Falling = KEEP; level not used
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(*ch, PCNT_CHANNEL_EDGE_ACTION_INCREASE,
                                                      PCNT_CHANNEL_LEVEL_ACTION_KEEP));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(*ch, PCNT_CHANNEL_LEVEL_ACTION_KEEP,
                                                       PCNT_CHANNEL_LEVEL_ACTION_KEEP));

    ESP_ERROR_CHECK(pcnt_unit_enable(*unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(*unit));
    ESP_ERROR_CHECK(pcnt_unit_start(*unit));
    return ESP_OK;
}

esp_err_t encoder_pcnt_create(gpio_num_t gpioA, gpio_num_t gpioB, encoder_pcnt_handle_t *out)
{
    if (!out) return ESP_ERR_INVALID_ARG;
    *out = NULL;

    encoder_pcnt_handle_t h = calloc(1, sizeof(*h));
    if (!h) return ESP_ERR_NO_MEM;

    esp_err_t err = make_rising_counter(gpioA, &h->unit_a, &h->cha);
    if (err != ESP_OK) { free(h); return err; }
    err = make_rising_counter(gpioB, &h->unit_b, &h->chb);
    if (err != ESP_OK) { /* leak-safe minimalism */ return err; }

    h->total_a = h->total_b = 0;
    *out = h;
    return ESP_OK;
}

esp_err_t encoder_pcnt_get_and_clear_delta_chA(encoder_pcnt_handle_t enc, int *delta)
{
    if (!enc || !delta) return ESP_ERR_INVALID_ARG;
    int c = 0;
    ESP_ERROR_CHECK(pcnt_unit_get_count(enc->unit_a, &c));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(enc->unit_a));
    enc->total_a += c;
    *delta = c;
    return ESP_OK;
}

esp_err_t encoder_pcnt_get_and_clear_delta_chB(encoder_pcnt_handle_t enc, int *delta)
{
    if (!enc || !delta) return ESP_ERR_INVALID_ARG;
    int c = 0;
    ESP_ERROR_CHECK(pcnt_unit_get_count(enc->unit_b, &c));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(enc->unit_b));
    enc->total_b += c;
    *delta = c;
    return ESP_OK;
}

esp_err_t encoder_pcnt_get_total_chA(encoder_pcnt_handle_t enc, int64_t *total)
{
    if (!enc || !total) return ESP_ERR_INVALID_ARG;
    int c = 0; pcnt_unit_get_count(enc->unit_a, &c);
    *total = enc->total_a + c;
    return ESP_OK;
}

esp_err_t encoder_pcnt_get_total_chB(encoder_pcnt_handle_t enc, int64_t *total)
{
    if (!enc || !total) return ESP_ERR_INVALID_ARG;
    int c = 0; pcnt_unit_get_count(enc->unit_b, &c);
    *total = enc->total_b + c;
    return ESP_OK;
}

esp_err_t encoder_pcnt_reset(encoder_pcnt_handle_t enc)
{
    if (!enc) return ESP_ERR_INVALID_ARG;
    enc->total_a = enc->total_b = 0;
    ESP_ERROR_CHECK(pcnt_unit_clear_count(enc->unit_a));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(enc->unit_b));
    return ESP_OK;
}
