#include <Arduino.h>
#include "CurrentSensor.h"

// Example: ACS712 5A -> 0.185 V/A, offset = 2.5V, spike threshold = 0.5A
CurrentSensor currentSensor(A0, 0.185, 2.5, 0.5);

void setup() {
    Serial.begin(9600);
}

void loop() {
    float current = currentSensor.read();
    Serial.print("Current: ");
    Serial.print(current, 3);
    Serial.println(" A");

    if (currentSensor.isSpike()) {
        Serial.println(" Spike detected!");
    }

    delay(500);
}
