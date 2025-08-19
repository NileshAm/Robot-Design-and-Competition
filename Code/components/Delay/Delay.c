#include "Delay.h"

void delay_ms(uint32_t delay){
    vTaskDelay(pdMS_TO_TICKS(delay));
    
}

void delay_s(uint32_t delay){
    vTaskDelay(pdMS_TO_TICKS(delay*1000));
}