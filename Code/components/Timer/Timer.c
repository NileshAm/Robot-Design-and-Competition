#include "Timer.h"

uint32_t millis()
{
    return esp_timer_get_time() / 1000;
}