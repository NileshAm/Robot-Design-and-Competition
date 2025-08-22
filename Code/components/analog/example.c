#include "analog.h"
#include "esp_log.h"
#include "Delay.h"

void app_main(void)
{
    // Configure a pin once (e.g., GPIO 34 on ADC1_CH6 on ESP32)
    analog_init(GPIO_NUM_34, ANALOG_ATTEN_12DB);

    while (1) {
        int v = analog_read(GPIO_NUM_4);  // raw 0..4095 at 12-bit
        if (v >= 0) {
            ESP_LOGI("APP", "ADC raw: %d\t ADC(mV): %f", v, v*(3.3f / 4095.0f) * 1000.0f);
        } else {
            ESP_LOGE("APP", "ADC read error %d", v);
        }
        delay_ms(200);
    }
}
