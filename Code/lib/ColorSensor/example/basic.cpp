#include <Arduino.h>
#include "ColorSensor.h"

// Pins: S0, S1, S2, S3, OUT
ColorSensor sensor(4, 5, 6, 7, 8);

void setup() {
    Serial.begin(115200);
    sensor.begin();

    // --- Calibration ---
    Serial.println("Place sensor on WHITE surface...");
    delay(3000); 
    sensor.calibrate(150, true); 

    Serial.println("Place sensor on BLACK surface...");
    delay(3000);
    sensor.calibrate(150, false);

    Serial.println("Calibration complete!");
}

void loop() {
    ColorName c = sensor.getColor();

    switch (c) {
        case COLOR_RED:   Serial.println("RED"); break;
        case COLOR_GREEN: Serial.println("GREEN"); break;
        case COLOR_BLUE:  Serial.println("BLUE"); break;
        case COLOR_BLACK: Serial.println("BLACK"); break;
        case COLOR_WHITE: Serial.println("WHITE"); break;
        default:          Serial.println("UNKNOWN"); break;
    }

    delay(200);
}
