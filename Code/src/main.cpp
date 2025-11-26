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
    
    // frontTof.disable();
    // leftTof.disable();
    // grabberTof.disable();
    // frontTopTof.disable();
    
    // frontTof.init(10);
    // leftTof.init(10);
    // grabberTof.init(10);
    // frontTopTof.init(10);
    
    ColorSensor grabberSensor(11,12, 13, 14, 15);
    ColorSensor boxColorSensor(16, 17, 18, 19, 20);
    
    OLED oled;
    if (!oled.begin()) {
        Serial.println("OLED init failed!");
        while (1);
    }
    
    Robot robot(leftMotor, rightMotor, ir , frontTof , leftTof , frontTopTof , grabberTof , grabberSensor , boxColorSensor , oled);

    // ---- collect min/max while you sweep over line/background ----
    // for (int i=0; i<200; ++i) {   // ~200 samples; adjust as needed
    //     ir.updateSensors();         // calls readRaw() & updates min/max
    //     delay(5);                   // small gap between samples
    // }
    // ir.calibrate();               // compute scale/offset from min/max


    // ---- Buttons ----
    // TODO: Verify these pin numbers!
    pushbutton btnUp(40);
    pushbutton btnDown(41);
    pushbutton btnSelect(42);

    btnUp.init();
    btnDown.init();
    btnSelect.init();
    Serial.println("button init done");

    // ---- Menu System ----
    MenuSystem menu(oled, btnUp, btnDown, btnSelect, grabberSensor, robot);
    menu.begin();

    // ---- Auto-Start Task 1 ----
    oled.clear();
    oled.display();
    Task1::run(robot); // Runs until interrupted
    Serial.println("task1 running");
    Serial.println("Exiting Task1");

    // ---- Show Menu After Interrupt ----
    menu.begin(); // Re-draw menu
    Serial.println("Entering Menu Loop");

    // ---- Main Loop ----
    while (true)
    {
        menu.update();
        
        // Optional: Keep sensor debug prints if needed, but they might slow down the menu
        // int raw[8];
        // ir.readRaw(raw);
        // ...
    }
}

void loop()
{
    // Empty because we have a while(true) in setup, 
    // but typically we should move the while(true) content here.
    // For now, keeping it in setup to match existing style, 
    // but the while(true) above handles the loop.
}
