#ifndef DELAY_H
#define DELAY_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void delay_ms(uint32_t delay);
void delay_s(uint32_t delay);

#endif