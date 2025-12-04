#include <Arduino.h>
#include <Motor.h>
#include <IRArray.h>
#include <Tof.h>
#include <OLED.h>
#include <ColorSensor.h>
#include <Grabber.h>
#include <ServoMotor.h>
#include <CurrentSensor.h>
#include <Robot.h>
#include <Utils.h>
#include <pushbutton.h>
#include <MenuSystem.h>
#include <UpdatePID.h>
#include <ServoMotor.h>

void setup()
{
    Serial.begin(9600);
    Serial.println("Setup started...");
    

    OLED oled;
    oled.begin();
    oled.displayText("Initializing...", 0, 0, 1);
    Serial.println("Initializing...");

    Motor rightMotor(28, 30, 5, 4, 2, 22, 450);  // dir1, dir2, pwmF, pwmB, encA, encB, ticks/rev   motor controller 1
    Motor leftMotor(32, 34, 6, 7, 3, 24, 450); // dir1, dir2, pwmF, pwmB, encA, encB, ticks/rev     motor controller 2
    leftMotor.init();
    rightMotor.init();
    oled.displayText("Motor init complete...", 0, 10, 1);
    
    const uint8_t PINS[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
    // equal weights (or set your own)
    const float W[8] = {0, 100, 200, 300, 400, 500, 600, 700};
    IRArray ir(8, PINS, W, 0.5f);
    ir.init();
    oled.displayText("IR init complete...", 0, 20, 1);

    Tof frontTof(40, 0x30, 20, 21);    // xshut, address, sda, scl
    Tof leftTof(44, 0x31, 20, 21);     // xshut, address, sda, scl
    Tof leftTof2(42, 0x34, 20, 21);    // xshut, address, sda, scl
    Tof rightTof(36, 0x32, 20, 21);    // xshut, address, sda, scl
    Tof frontTopTof(38, 0x33, 20, 21); // xshut, address, sda, scl

    frontTof.disable();
    leftTof.disable();
    leftTof2.disable();
    rightTof.disable();
    frontTopTof.disable();

    frontTof.init(19);
    leftTof.init(2);
    leftTof2.init(-21);
    rightTof.init(17);
    frontTopTof.init(39);
    oled.displayText("TOF init complete...", 0, 30, 1);

    ColorSensor boxColorSensor(11, 12, 13, 14, 15, 61);
    ColorSensor grabberSensor(35, 37, 31, 33, 39, 41);
    boxColorSensor.begin();
    grabberSensor.begin();
    oled.displayText("Color init complete...", 0, 40, 1);
    
    pushbutton btnUp(50);
    pushbutton btnDown(48);
    pushbutton btnSelect(52);
    btnUp.init();
    btnDown.init();
    btnSelect.init();
    oled.clear();
    oled.displayText("Button init complete...", 0, 0, 1); 
    
    // --- Grabber Initialization ---
    ServoMotor grabServo(8); 
    ServoMotor liftServo(9);
    CurrentSensor currentSensor(A8, 0.001, 60); 
    Grabber grabber(grabServo, liftServo, currentSensor);
    grabber.init();

    Robot robot(leftMotor, rightMotor, ir, frontTof, leftTof, leftTof2, frontTopTof, rightTof, grabberSensor, boxColorSensor, grabber, oled);
    oled.displayText("Robot init complete...", 0, 10, 1);


    oled.clear();
    MenuSystem *menu;
    menu = new MenuSystem(oled, btnUp, btnDown, btnSelect, grabberSensor, robot);

    // Initialize Menu
    menu->begin();
    while (true)
    {
        menu->update();
        delay(10);
    }

}

void loop()
{
    // Empty because we have a while(true) in setup,
    // but typically we should move the while(true) content here.
    // For now, keeping it in setup to match existing style,
    // but the while(true) above handles the loop.
}

