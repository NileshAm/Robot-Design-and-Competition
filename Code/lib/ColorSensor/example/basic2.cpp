#include <Arduino.h>
#include "ColorSensor.h"

// Pins: S0, S1, S2, S3, OUT, OE
ColorSensor sensor(4, 5, 6, 7, 8,9);

void setup() {
    Serial.begin(9600);
    sensor.begin();

    // --- Auto Calibration ---
    sensor.calibrate();
    
    // Wait 1 second before starting loop
    delay(1000);
}

unsigned long lastColorRead = 0;
const unsigned long COLOR_READ_INTERVAL = 500; // 2 seconds

void loop() {
    if (millis() - lastColorRead >= COLOR_READ_INTERVAL) {
        lastColorRead = millis();
        
        // Burst read to allow smoothing and hysteresis to settle
        ColorName c = COLOR_UNKNOWN;
        for (int i = 0; i < 10; i++) {
            c = sensor.getColor();
            delay(10); 
        }

        switch (c) {
            case COLOR_RED:   Serial.println("RED"); break;
            case COLOR_GREEN: Serial.println("GREEN"); break;
            case COLOR_BLUE:  Serial.println("BLUE"); break;
            case COLOR_BLACK: Serial.println("BLACK"); break;
            case COLOR_WHITE: Serial.println("WHITE"); break;
            default:          Serial.println("UNKNOWN"); break;
        }
    }
}
