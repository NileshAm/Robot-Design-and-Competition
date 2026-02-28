#include "MenuSystem.h"
#include <Arduino.h>
#include <Utils.h>
#include "../../src/Tasks/Task2/Task2.h"

MenuSystem::MenuSystem(OLED &oled,
                       pushbutton &btnUp,
                       pushbutton &btnDown,
                       pushbutton &btnSelect,
                       ColorSensor &sensor,
                       Robot &robot)
    : _oled(oled),
      _btnUp(btnUp),
      _btnDown(btnDown),
      _btnSelect(btnSelect),
      _sensor(sensor),
      _robot(robot)
{
}

void MenuSystem::begin()
{
    _robot.setInterruptButton(_btnSelect);
    drawMenu();
}

// -------------------------------
// Update page according to scroll
// -------------------------------
void MenuSystem::updatePage()
{
    if (currentIndex < pageStart)
        pageStart = currentIndex;

    else if (currentIndex >= pageStart + itemsPerPage)
        pageStart = currentIndex - (itemsPerPage - 1);
}

// -------------------------------
// Draw 3 items starting at pageStart
// -------------------------------
void MenuSystem::drawMenu()
{
    _oled.clear();

    for (int i = 0; i < itemsPerPage; i++)
    {
        int itemIndex = pageStart + i;
        if (itemIndex >= menuCount)
            break;

        if (itemIndex == currentIndex)
            _oled.displayText("> " + menuItems[itemIndex], 0, i * 12);
        else
            _oled.displayText("  " + menuItems[itemIndex], 0, i * 12);
    }

    _oled.display();
}

// -------------------------------
// Allow running a task
// -------------------------------
void MenuSystem::runTask(const String &name, void (*fn)(Robot &))
{
    _oled.clear();
    _oled.displayCenteredText("Running " + name, 1);
    _oled.display();
    delay(800);

    if (fn != nullptr)
        fn(_robot);

    _oled.clear();
    _oled.displayCenteredText(name + " Done", 1);
    _oled.display();
    delay(1500);
}

// -------------------------------
// Calibration handler
// -------------------------------
void MenuSystem::lineFollow()
{
    _oled.clear();
    _oled.displayCenteredText("line following.....", 1);
    _oled.display();

    while (true)
    {
        _robot.followLine();
    }

    _oled.clear();
    _oled.displayCenteredText("Done", 1);
    _oled.display();
    delay(1500);
}

void MenuSystem::calibrateIR()
{
    _oled.clear();
    _oled.displayCenteredText("Calibrating...", 1);
    _oled.display();

    _robot.calibrateIR();

    _oled.clear();
    _oled.displayCenteredText("Calibration OK", 1);
    _oled.display();
    delay(1500);
}
void MenuSystem::calibrateColor()
{
    _oled.clear();
    _oled.displayCenteredText("Place on RED", 1);
    _oled.display();
    delay(3000);
    _robot.grabberSensor.calibrateTarget(COLOR_RED, 150);

    _oled.clear();
    _oled.displayCenteredText("Place on GREEN", 1);
    _oled.display();
    delay(3000);
    _robot.grabberSensor.calibrateTarget(COLOR_GREEN, 150);

    _oled.clear();
    _oled.displayCenteredText("Place on BLUE", 1);
    _oled.display();
    delay(3000);
    _robot.grabberSensor.calibrateTarget(COLOR_BLUE, 150);

    _oled.clear();
    _oled.displayCenteredText("Place on WHITE", 1);
    _oled.display();
    delay(3000);
    _robot.grabberSensor.calibrateTarget(COLOR_WHITE, 150);

    _oled.clear();
    _oled.displayCenteredText("Place on BLACK", 1);
    _oled.display();
    delay(3000);
    _robot.grabberSensor.calibrateTarget(COLOR_BLACK, 150);

    _robot.grabberSensor.saveCalibration();

    _oled.clear();
    _oled.displayCenteredText("Done", 1);
    _oled.display();
    delay(1500);
    drawMenu();
}

void MenuSystem::singleWallFollow()
{
    _oled.clear();
    _oled.displayCenteredText("Single wall follow...", 1);
    _oled.display();

    _robot.MotorL.resetTicks();
    _robot.MotorR.resetTicks();
    while (true)
    {
        _robot.followSingleWall();
    }
    _robot.brake();
    _oled.clear();
    _oled.displayCenteredText("Task Done", 1);
    _oled.display();
    delay(1500);
}
void MenuSystem::test()
{
    _oled.clear();
    _oled.displayCenteredText("Testing...", 1);

    Task2::run(_robot);
    while (true)
    {
        delay(200);
    }
    int initAngle = _robot.imu.getYaw();
    bool rightcheck = true;
    while (true)
    {
        if(_robot.junction.isLine()){
            _robot.followLine(25);
            initAngle = _robot.imu.getYaw();
            _robot.MotorL.resetTicks();
            _robot.MotorR.resetTicks();
        }
        else{
            if (_robot.junction.isRightTurn())
            {
                _robot.turnRight();
            }
            else if (_robot.junction.isLeftTurn())
            {
                _robot.turnLeft();
            }
            else{
                if((_robot.MotorL.getTicks()+_robot.MotorR.getTicks())/2 < 350)
                {
                    _robot.moveStraightGyro(initAngle, 25);
                }
                else{
                    while ((_robot.MotorL.getTicks()+_robot.MotorR.getTicks())/2 > 100)
                    {
                        _robot.moveStraightGyro(initAngle, -25);
                    }
                    if(rightcheck){
                        _robot.turn(15);
                    }else{
                        _robot.turn(-30);
                    }
                    initAngle = _robot.imu.getYaw();
                }
            }
        }
    }
    _robot.brake();
    
}
void MenuSystem::ramp()
{
    _oled.clear();
    _oled.displayCenteredText("Ramp running", 1);
    _oled.display();

    while (_robot.junction.isAllBlack())
    {
        _robot.followRamp(-40);
    }
    _robot.brake();
}
void MenuSystem::objectDetect()
{
    _oled.clear();
    _oled.displayCenteredText("Object detection running", 1);
    _oled.display();

    while (true)
    {
        _oled.clear();
        _oled.displayText("Front: " + String(_robot.detectFrontBox()), 0, 0, 1);
        _oled.displayText("Left: " + String(_robot.detectLeftBox()), 0, 10, 1);
        _oled.displayText("Right: " + String(_robot.detectRightBox()), 0, 20, 1);
        _oled.displayText("Obstacle: " + String(_robot.detectObstacle()), 0, 30, 1);
        delay(100);
    }
}

void MenuSystem::detectColor()
{
    while (true)
        {
            delay(500);
            switch (_robot.grabberSensor.getColor())
            {
            case COLOR_RED:
                _oled.clear();
                Serial.println("RED");
                _oled.displayText("RED");
                _oled.display();
                break;
            case COLOR_GREEN:
                _oled.clear();
                Serial.println("GREEN");
                _oled.displayText("GREEN");
                _oled.display();
                break;
            case COLOR_BLUE:
                _oled.clear();
                Serial.println("BLUE");
                _oled.displayText("BLUE");
                _oled.display();
                break;
            case COLOR_BLACK:
                _oled.clear();
                Serial.println("BLACK");
                _oled.displayText("BLACK");
                _oled.display();
                break;
            case COLOR_WHITE:
                _oled.clear();
                Serial.println("WHITE");
                _oled.displayText("WHITE");
                _oled.display();
                break;
            default:
                _oled.clear();
                Serial.println("detecting Color");
                _oled.displayText("detecting Color");
                _oled.display();
                break;
            }
        }
}

// FIX: Make the values scroll down
void MenuSystem::debugTOF()
{
    while (true)
    {
        _oled.clear();
        _oled.displayText("Front", 0, 0, 1);
        _oled.displayText("Top", 30, 0, 1);
        _oled.displayText("left", 60, 0, 1);
        _oled.displayText("left2", 90, 0, 1);
        _oled.displayText("right", 0, 10, 1);

        _oled.displayText((String)_robot.frontTof.readRange(), 0, 20, 1);
        _oled.displayText((String)_robot.frontTopTof.readRange(), 30, 20, 1);
        _oled.displayText(((String)_robot.leftTof.readRange()), 60, 20, 1);
        _oled.displayText((String)_robot.leftTof2.readRange(), 90, 20, 1);
        _oled.displayText((String)_robot.rightTof.readRange(), 0, 30, 1);

        delay(100);
    }
}
// FIX: Make the values scroll down
void MenuSystem::debugIR()
{
    while (true)
    {
        _oled.clear();
        bool dig[8];
        _robot.ir.readDigital(dig);
        for (int i = 0; i < 8; i++)
        {
            _oled.displayText((String)dig[i], i * 10, 10, 1);
        }
        delay(100);
    }
}

void MenuSystem::straightLine()
{
    int initYaw = _robot.imu.getYaw();
    while (true)
    {
        // _oled.clear();
        // _oled.displayText((String)_robot.MotorR.getTicks(), 0, 0, 1);
        // _oled.displayText((String)_robot.MotorL.getTicks(), 10, 0, 1);
        // _robot.moveStraightGyro(30);
        _robot.moveStraightGyro(initYaw, 30);
    }
}

void MenuSystem::gridRun()
{
    _oled.clear();
    _oled.displayText("Grid run...", 0, 0, 1);
    delay(1000);

    int initAngle = _robot.imu.getYaw();
    bool rightcheck = true;
    while (true)
    {
        if(_robot.junction.isLine()){
            _robot.followLine(25);
            initAngle = _robot.imu.getYaw();
            _robot.MotorL.resetTicks();
            _robot.MotorR.resetTicks();
        }
        else{
            if (_robot.junction.isRightTurn())
            {
                _robot.turnRight();
            }
            else if (_robot.junction.isLeftTurn())
            {
                _robot.turnLeft();
            }
            else{
                if((_robot.MotorL.getTicks()+_robot.MotorR.getTicks())/2 < 800)
                {
                    _robot.moveStraightGyro(initAngle, 25);
                }
                else{
                    while ((_robot.MotorL.getTicks()+_robot.MotorR.getTicks())/2 > 100)
                    {
                        _robot.moveStraightGyro(initAngle, -25);
                    }
                    if(rightcheck){
                        _robot.turn(15);
                    }else{
                        _robot.turn(-30);
                    }
                    initAngle = _robot.imu.getYaw();
                }
            }
        }

    }
    _robot.brake();
    delay(1500);
}

void MenuSystem::garbBall()
{
    _oled.clear();
    _oled.displayCenteredText("Grabbing ball...", 1);
    _oled.display();
    delay(1000);

    while (true)
    {
        if (_robot.grabber.grab())
        {
            Serial.println("grab");
            delay(1000);
            _robot.grabber.lift();
            delay(1000);
            _robot.grabber.release();
            delay(1000);
            _robot.grabber.reset();
            delay(1000);
        }
        _robot.grabber.release();
    }
}

void MenuSystem::GrabBox()
{
    _oled.clear();
    _oled.displayCenteredText("Grabbing box...", 1);
    _oled.display();
    delay(1000);

    while (true)
    {
        if (_robot.grabber.grab())
        {
            Serial.println("grab");
            delay(2000);
            _robot.grabber.liftBox();
            delay(2000);
            _robot.grabber.reset();
            delay(2000);
        }
        _robot.grabber.release();
    }
}

void MenuSystem::runTask1()
{
    runTask("Task 1", Task1::run);
}
void MenuSystem::runTask2()
{
    runTask("Task 2", Task2::run);
}
void MenuSystem::runTask3()
{
    runTask("Task 3", Task3::run);
}
void MenuSystem::runTask4()
{
    runTask("Task 4", Task4::run);
}   
void MenuSystem::runTask5()
{
    runTask("Task 5", Task5::run);
}

// -------------------------------
// Main update loop
// -------------------------------
void MenuSystem::update()
{

    // ---- UP BUTTON (falling edge) ----
    if (_btnUp.stateChanged() == 1)
    {
        currentIndex--;
        if (currentIndex < 0)
            currentIndex = menuCount - 1;
        updatePage();
        drawMenu();
    }

    // ---- DOWN BUTTON (falling edge) ----
    if (_btnDown.stateChanged() == 1)
    {
        currentIndex++;
        if (currentIndex >= menuCount)
            currentIndex = 0;
        updatePage();
        drawMenu();
    }

    // ---- SELECT BUTTON (falling edge) ----
    if (_btnSelect.stateChanged() == 1)
    {

        switch (currentIndex)
        {
        case 0:
            // calibrateIR();
            test();
            break;

        case 1:
            lineFollow();
            break;
        case 2:
            straightLine();
            break;

        case 3:
            ramp();
            break;

        case 4:
            singleWallFollow();
            break;

        case 5:
            objectDetect();
            break;

        case 6:
            detectColor();
            break;

        case 7:
            garbBall();
            break;
        case 8:
            GrabBox();
            break;
        case 9:
            calibrateIR();
            break;
        case 10:
            calibrateColor();
            break;
        case 11:
            gridRun();
            break;
        case 12:
            delay(1000);
            break;
        case 13:
            debugTOF();
            break;
        case 14:
            debugIR();
            break;
        case 15:
            runTask1();
            break;
        case 16:
            runTask("Bypass Test", Task1::runDynamicBypassTest);
            break;
        case 17:
            runTask("Traverse", Traverse::run);
            break;

        }
        drawMenu();
    }
}


