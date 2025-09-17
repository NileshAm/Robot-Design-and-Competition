#include <Arduino.h>
#include "ColorSensor.h"

// Wiring: TCS3200/TCS230 example
// S0 -> 4, S1 -> 5, S2 -> 6, S3 -> 7, OUT -> 8
ColorSensor sensor(4, 5, 6, 7, 8);

void setup() {
    Serial.begin(9600);
    sensor.begin();
    sensor.calibrate();
    Serial.println("Calibration done!");
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

    delay(500);
}
