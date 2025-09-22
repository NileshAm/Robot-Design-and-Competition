#include <Arduino.h>
#include "UltraSonic.h"


#define ECHO_PIN  2
#define TRIG_PIN  3

//echopin,trigpin

UltraSonic sensor(ECHO_PIN, TRIG_PIN);

void setup() {
    Serial.begin(9600);
    sensor.init();
    Serial.println("initialized");
}

void loop() {
    float distance = sensor.getDistance();

    if (distance > 0) {
        Serial.println(distance);
    } else {
        Serial.println("out of range");
    }

    delay(500);
}
