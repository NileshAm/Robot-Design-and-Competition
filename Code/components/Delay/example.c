#include "Delay.h"
#include <stdio.h>

void app_main(void)
{
    while (true)
    {
        printf("Action performed\n");
        delay(1000); // Delay for 1000 milliseconds (1 second)
        printf("Action completed after delay\n");
        delay_s(2); // Delay for 2 seconds
        printf("Action completed after 2 seconds delay\n");
    }
}