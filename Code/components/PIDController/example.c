#include "PIDController.h"

void app_main(){
    PID pid;
    initPID(&pid, 1.0f, 0.1f, 0.01f);

    
    // Set new PID parameters
    setP(&pid, 2.0f);
    setI(&pid, 0.2f);
    setD(&pid, 0.02f);
    
    float output = 0.0f;
    while(1)
    {
        // Calculate with new parameters
        float error = 5.0f; // Example error value
        output = calculatePID(&pid, error);
        printf("New PID Output: %.2f\n", output);
    }
    
}