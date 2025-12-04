#include <Arduino.h>
#include <CurrentSensor.h>
#include <ServoMotor.h>
#include <Grabber.h>

ServoMotor grabberServo(9);   
ServoMotor lifterServo(10);   

CurrentSensor currentSensor(A0, 0.001, 30);  

Grabber grabber = Grabber(grabberServo, lifterServo, currentSensor);


void setup() {
    Serial.begin(9600);

    Serial.println("init");
    grabber.init();
    Serial.println("init finished");
    while (true)
    {
        Serial.println("releasing");
        grabber.release();
        if (grabber.grab()){
            delay(2000);
            grabber.lift();
            delay(1000);
            grabber.release();
            grabber.reset();
        }
        
    }
    
}
void loop() {



}
