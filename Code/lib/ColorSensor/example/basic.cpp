#include <Arduino.h>
#include "ColorSensor.h"
#include <OLED.h>

OLED oled;
// Pins: S0, S1, S2, S3, OUT
ColorSensor sensor(4, 5, 6, 7, 8);

void setup() {
    Serial.begin(9600);
    sensor.begin();

    // --- Calibration ---
    oled.displayText("Place sensor on WHITE surface...");
    oled.display();
    Serial.println("Place sensor on WHITE surface...");
    delay(3000); 
    sensor.calibrate(150, true); 
    oled.clear();

    oled.displayText("Place sensor on BLACK surface...");
    oled.display();
    Serial.println("Place sensor on BLACK surface...");
    delay(3000);
    sensor.calibrate(150, false);
    oled.clear();

    oled.displayText("Calibration complete!");
    oled.display();
    Serial.println("Calibration complete!");
    oled.clear();
}

void loop() {
    ColorName c = sensor.getColor();

    switch (c) {
        case COLOR_RED:   
            oled.clear();
            Serial.println("RED");
            oled.displayText("RED");
            oled.display(); 
            break;
        case COLOR_GREEN: 
            oled.clear();
            Serial.println("GREEN"); 
            oled.displayText("RED");
            oled.display();
            break;
        case COLOR_BLUE:  
            oled.clear();
            Serial.println("BLUE"); 
            oled.displayText("RED");
            oled.display();
            break;
        case COLOR_BLACK: 
            oled.clear();
            Serial.println("BLACK");
            oled.displayText("BLACK");
            oled.display(); 
            break;
        case COLOR_WHITE: 
            oled.clear();
            Serial.println("WHITE");
            oled.displayText("WHITE");
            oled.display();
            break;
        default:          
            oled.clear();
            Serial.println("UNKNOWN");
            oled.displayText("UNKOWN");
            oled.display(); 
            break;
    }

    delay(200);
}
