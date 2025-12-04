#include <Arduino.h>
#include "CurrentSensor.h"

// Create sensor object: pin A2, sensitivity 0.185V/A, spike threshold 0.25A
CurrentSensor currentSensor(A0, 0.185, 0.25);

void setup() {
    Serial.begin(9600);

    // Calibrate zero-offset (optional but good)
    currentSensor.begin();  
}

void loop() {
    float current = currentSensor.read();  // read current in Amps

    Serial.print("Current: ");
    Serial.print(current, 3);  // print 3 decimal places
    Serial.println(" A");

    delay(500);  // 1 second delay
}
