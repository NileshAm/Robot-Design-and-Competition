#include "Timer.h"
#include <stdio.h>

void app_main(void)
{
    uint32_t start_time = millis();
    printf("Some action performed\n");
    uint32_t end_time = millis();
    printf("Action completed after delay. Time taken: %" PRIu32 " ms\n", end_time - start_time);
}