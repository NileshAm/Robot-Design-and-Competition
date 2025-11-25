#include <Arduino.h>
#include <Motor.h>
#include <IRArray.h>
#include <Tof.h>
#include <OLED.h>
#include <ColorSensor.h>
#include <Robot.h>
#include <Utils.h>




void setup()
{
    Serial.begin(9600);

    Motor leftMotor(38, 36, 4, 2, 30, 600);     // dir1, dir2, pwm, encA, encB, ticks/rev
    Motor rightMotor(22, 28, 5, 3, 24, 600); // dir1, dir2, pwm, encA, encB, ticks/rev
    leftMotor.init();
    rightMotor.init();
    
    const uint8_t PINS[8] = {A0,A1,A2,A3,A4,A5,A6,A7};
    // equal weights (or set your own)
    const float   W[8]    = {0,100,200,300,400,500,600,700};
    IRArray ir(8, PINS, W, 0.5f);
    ir.init();
    
    Tof frontTof(7, 0x29, 20, 21); // xshut, address, sda, scl
    Tof leftTof(8, 0x30, 20, 21); // xshut, address, sda, scl
    Tof grabberTof(9, 0x31, 20, 21); // xshut, address, sda, scl
    Tof frontTopTof(10, 0x32, 20, 21); // xshut, address, sda, scl
    
    frontTof.disable();
    leftTof.disable();
    grabberTof.disable();
    frontTopTof.disable();
    
    frontTof.init(10);
    leftTof.init(10);
    grabberTof.init(10);
    frontTopTof.init(10);
    
    ColorSensor grabberSensor(11,12, 13, 14, 15);
    ColorSensor boxColorSensor(16, 17, 18, 19, 20);
    
    OLED oled;
    if (!oled.begin()) {
        Serial.println("OLED init failed!");
        while (1);
    }
    
    Robot robot(leftMotor, rightMotor, ir , frontTof , leftTof , frontTopTof , grabberTof , grabberSensor , boxColorSensor , oled);

    // ---- collect min/max while you sweep over line/background ----
    for (int i=0; i<200; ++i) {   // ~200 samples; adjust as needed
        ir.updateSensors();         // calls readRaw() & updates min/max
        delay(5);                   // small gap between samples
    }
    ir.calibrate();               // compute scale/offset from min/max

    while (true)
    {
        int raw[8];
        double    norm[8];
        bool     dig[8];

        ir.readRaw(raw);
        ir.readNormalized(norm);  // internally calls readRaw()
        ir.digitalRead(dig);      // internally calls readNormalized()


        // printArray(raw, 8, ", ");
        // Serial.println();
        // printArray(norm, 8, ", ");
        // Serial.println();
        printArray(dig, 8, ", ");
        // Serial.println(robot.ir.weightedSum()-350);
        // Serial.println();

        // delay(100);
        // leftMotor.setSpeed(50);
        // rightMotor.setSpeed(50);

        // Serial.print(robot.MotorL.getTicks());      // delay(2000);
        // Serial.print("\t");                         // delay(2000);
        // Serial.println(robot.MotorR.getTicks());      // delay(2000);
        // leftMotor.setSpeed(0);
        // rightMotor.setSpeed(0);
        // delay(2000);
        // leftMotor.setSpeed(-50);
        // rightMotor.setSpeed(-50);
        // delay(2000);
        // leftMotor.setSpeed(0);
        // rightMotor.setSpeed(0);
        // delay(2000);
        // leftMotor.setSpeed(50);
        // rightMotor.setSpeed(-50);
        // delay(2000);
        // leftMotor.setSpeed(0);
        // rightMotor.setSpeed(0);
        // delay(2000);
        // leftMotor.setSpeed(-50);
        // rightMotor.setSpeed(50);
        // delay(2000);
        // leftMotor.setSpeed(0);
        // rightMotor.setSpeed(0);
        // delay(2000);
    }
    

    
}

void loop()
{
    
}
