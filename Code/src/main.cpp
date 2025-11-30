#include <Arduino.h>
#include <Motor.h>
#include <IRArray.h>
#include <Tof.h>
#include <OLED.h>
#include <ColorSensor.h>
#include <Robot.h>
#include <Utils.h>
#include <pushbutton.h>
#include <MenuSystem.h>
#include <UpdatePID.h>

void setup()
{
    Serial.begin(9600);
    Serial2.begin(9600); // ESP32 communication

    OLED oled;
    oled.begin();
    oled.displayText("Initializing...", 0, 0, 1);    
    
    Motor rightMotor(44, 42, 4, 2, 46, 600);     // dir1, dir2, pwm, encA, encB, ticks/rev
    Motor leftMotor(36, 38, 5, 3, 40, 600); // dir1, dir2, pwm, encA, encB, ticks/rev
    leftMotor.init();
    rightMotor.init();
    oled.displayText("Motor init complete...", 0, 10, 1);    
    
    const uint8_t PINS[8] = {A0,A1,A2,A3,A4,A5,A6,A7};
    // equal weights (or set your own)
    const float   W[8]    = {0,100,200,300,400,500,600,700};
    IRArray ir(8, PINS, W, 0.5f);
    ir.init();
    oled.displayText("IR init complete...", 0, 20, 1);    
    
    Tof frontTof(26, 0x30, 20, 21); // xshut, address, sda, scl
    Tof leftTof(24, 0x31, 20, 21); // xshut, address, sda, scl
    Tof leftTof2(32, 0x34, 20, 21); // xshut, address, sda, scl
    Tof rightTof(28, 0x32, 20, 21); // xshut, address, sda, scl
    Tof frontTopTof(30, 0x33, 20, 21); // xshut, address, sda, scl
    
    frontTof.disable();
    leftTof.disable();
    leftTof2.disable();
    rightTof.disable();
    frontTopTof.disable();
    
    frontTof.init(20);
    leftTof.init(24);
    leftTof2.init(-9);
    rightTof.init(16);
    frontTopTof.init(29);
    oled.displayText("TOF init complete...", 0, 30, 1);    
    
    ColorSensor grabberSensor(11,12, 13, 14, 15);
    ColorSensor boxColorSensor(35, 37, 31, 33, 29);
    boxColorSensor.begin();
    oled.displayText("Color init complete...", 0, 40, 1);    
    
    pushbutton btnUp(50);
    pushbutton btnDown(48);
    pushbutton btnSelect(52);
    btnUp.init();
    btnDown.init();
    btnSelect.init();
    oled.clear();
    oled.displayText("Button init complete...", 0, 0, 1); 
    
    Robot robot(leftMotor, rightMotor, ir, frontTof, leftTof, leftTof2, frontTopTof, rightTof, grabberSensor, boxColorSensor, oled);
    oled.displayText("Robot init complete...", 0, 10, 1);

    oled.clear();
    MenuSystem *menu;
    menu = new MenuSystem(oled, btnUp, btnDown, btnSelect, grabberSensor, robot);
    menu->begin();
    while (true)
    {
        menu->update();
        delay(10);
    }
    
    oled.clear();
    delay(500);
    
    // ---- PID Update Check ----
    UpdatePID pidUpdater(Serial2);
    oled.clear();
    oled.displayCenteredText("Waiting for PID...", 1);
    
    unsigned long startTime = millis();
    while (millis() - startTime < 2000) {
        pidUpdater.update();
        if (pidUpdater.isUpdated()) {
            break;
        }
    }

    if (pidUpdater.isUpdated()) {
        oled.clear();
        oled.displayCenteredText("PID Updated!", 2);
        
        // Update Robot PID
        // Note: Robot is initialized below, so we need to store values or init robot earlier.
        // Since robot depends on motors/sensors which are already init, we can move robot init up 
        // OR just set the values after robot init.
        // Let's set a flag and values to update after robot creation.
    } else {
        oled.clear();
        oled.displayCenteredText("No PID Update", 1);
    }
    delay(500);


    
    if (pidUpdater.isUpdated()) {
        robot.setLineFollowerPID(pidUpdater.getKp(), pidUpdater.getKi(), pidUpdater.getKd());
        oled.clear();
        oled.displayText("Kp: " + String(pidUpdater.getKp()), 0, 0, 1);
        oled.displayText("Ki: " + String(pidUpdater.getKi()), 0, 10, 1);
        oled.displayText("Kd: " + String(pidUpdater.getKd()), 0, 20, 1);
        delay(5000);
    }
    

    oled.clear();
    oled.displayCenteredText("Calibrating IR...", 1);
        // ---- collect min/max while you sweep over line/background ----
    // for (int i=0; i<20; ++i) {   // ~200 samples; adjust as needed
    //     ir.updateSensors();         // calls readRaw() & updates min/max
    //     delay(5);                   // small gap between samples
    // }
    // ir.calibrate();               // compute scale/offset from min/max
    oled.clear();
    oled.displayCenteredText("IR Calibrated", 1);
    delay(2000);

    // ---- Buttons ----
    // TODO: Verify these pin numbers!
    // pushbutton btnUp(40);
    // pushbutton btnDown(41);
    // pushbutton btnSelect(42);

    // btnUp.init();
    // btnDown.init();
    // btnSelect.init();
    // Serial.println("button init done");

    // // ---- Menu System ----
    // MenuSystem menu(oled, btnUp, btnDown, btnSelect, grabberSensor, robot);
    // menu.begin();

    // // ---- Auto-Start Task 1 ----
    // oled.clear();
    // oled.display();
    // Task1::run(robot); // Runs until interrupted
    // Serial.println("task1 running");
    // Serial.println("Exiting Task1");

    // // ---- Show Menu After Interrupt ----
    // menu.begin(); // Re-draw menu
    // Serial.println("Entering Menu Loop");

    // ---- Main Loop ----
    oled.clear();
    oled.displayCenteredText("starting to follow line", 1);
    delay(1000);
    while (robot.leftTof2.readRange() < 200)
    {
        Serial.println(btnDown.buttonStatus());
        // Serial.print(leftMotor.getTicks());
        // Serial.print("\t");
        // Serial.println(rightMotor.getTicks());
        // robot.followLine();
        // robot.followSingleWall();
        // Serial.print(rightTof.readRange());
        // Serial.print("\t");
        // Serial.print(leftTof.readRange());
        // Serial.print("\t");
        // Serial.print(leftTof2.readRange());
        // Serial.print("\t");
        // Serial.print(frontTof.readRange());
        // Serial.print("\t");
        // Serial.println(frontTopTof.readRange());
        // menu.update();
        
        // Optional: Keep sensor debug prints if needed, but they might slow down the menu
        // int raw[8];
        // ir.readRaw(raw);
        // ...
        // Check for PID updates/requests
        // pidUpdater.update();
        // if (pidUpdater.isUpdated()) {
        //     int type = pidUpdater.getType();
        //     float kp = pidUpdater.getKp();
        //     float ki = pidUpdater.getKi();
        //     float kd = pidUpdater.getKd();

        //     oled.clear();
        //     oled.displayText("PID Updated!", 0, 0, 1);
            
        //     if (type == 0) {
        //         robot.setLineFollowerPID(kp, ki, kd);
        //         oled.displayText("Type: Line", 0, 10, 1);
        //     } else if (type == 1) {
        //         robot.setWallFollowerPID(kp, ki, kd);
        //         oled.displayText("Type: Wall", 0, 10, 1);
        //     } else if (type == 2) {
        //         robot.setStraightLinePID(kp, ki, kd);
        //         oled.displayText("Type: Straight", 0, 10, 1);
        //     }

        //     oled.displayText("Kp: " + String(kp), 0, 20, 1);
        //     oled.displayText("Ki: " + String(ki), 0, 30, 1);
        //     oled.displayText("Kd: " + String(kd), 0, 40, 1);
        //     pidUpdater.resetUpdated();
        //     delay(2000);
        // }

        delay(10);
    }
    robot.stop();
}

void loop()
{
    // Empty because we have a while(true) in setup, 
    // but typically we should move the while(true) content here.
    // For now, keeping it in setup to match existing style, 
    // but the while(true) above handles the loop.
}
