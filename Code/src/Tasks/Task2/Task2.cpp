#include "Task2.h"

namespace Task2
{
    void run(Robot &robot)
    {
        
        robot.oled.displayText("Task2 Start", 0, 0, 1);
        robot.oled.display();
        bool wasOnLine = robot.junction.isLine();
        int initAngle = robot.imu.getYaw();
        
        while (!robot.junction.isAllWhite())
        {
            if (robot.junction.isLine())
            {
                wasOnLine = true;
                robot.followLine(25);
                robot.MotorL.resetTicks();
                robot.MotorR.resetTicks();
            }
            else if (robot.junction.isRightEdge()){
                robot.turn(10);
            }
            else if (robot.junction.isLeftEdge())
            {
                robot.turn(-10);
            }
            else
            {
                if (wasOnLine)
                {
                    robot._lineFollowerPID.reset();
                    initAngle = robot.imu.getYaw();
                    wasOnLine = false;
                }
                
                if ((robot.MotorL.getTicks() + robot.MotorR.getTicks()) / 2 < 375)
                {
                    robot.moveStraightGyro(initAngle, 20);
                }
                else
                {
                    robot.MotorL.resetTicks();
                    robot.MotorR.resetTicks();
                    while (labs(robot.MotorL.getTicks() + robot.MotorR.getTicks()) / 2 < 350)
                    {
                        robot.moveStraightGyro(initAngle, -25);
                    }
                    robot.turn(-20);
                    initAngle = robot.imu.getYaw();
                }
            }
        }
        wasOnLine = false;
        robot.brake();
        robot.goTillCM(31.5);
        robot.goTillCM(-10);
        robot.turn(180);
        
        Serial.println("turned");
        initAngle = robot.imu.getYaw();
        Serial.println(robot.imu.getYaw());
        Serial.println(initAngle - 180);

        Serial.println("till line");
        while (!robot.junction.isLine())
        {
            robot.moveStraightGyro(initAngle);
        }
        Serial.println("2.5");
        robot.goTillCM(5);

        while (!robot.junction.isAllWhite())
        {
            if (robot.junction.isLine())
            {
                wasOnLine = true;
                robot.followLine(25);
                robot.MotorL.resetTicks();
                robot.MotorR.resetTicks();
            }
            else if (robot.junction.isRightEdge()){
                robot.turn(10);
            }
            else if (robot.junction.isLeftEdge())
            {
                robot.turn(-10);
            }
            else
            {
                if (wasOnLine)
                {
                    robot._lineFollowerPID.reset();
                    initAngle = robot.imu.getYaw();
                    wasOnLine = false;
                }
                
                if ((robot.MotorL.getTicks() + robot.MotorR.getTicks()) / 2 < 250)
                {
                    robot.moveStraightGyro(initAngle, 20);
                }
                else
                {
                    robot.MotorL.resetTicks();
                    robot.MotorR.resetTicks();
                    while (labs(robot.MotorL.getTicks() + robot.MotorR.getTicks()) / 2 < 225)
                    {
                        robot.moveStraightGyro(initAngle, -25);
                    }
                    robot.turn(20);
                    initAngle = robot.imu.getYaw();
                }
            }
        }
        robot.brake();
        robot.goTillCM(-35);
        robot.turnLeft();
        robot.MotorL.resetTicks();
        robot.MotorR.resetTicks();       
        bool isFirst = true;
        while (robot.junction.isAllBlack() || isFirst)
        {
            robot.followRamp(-40);
        }
        robot.brake();
        robot.followRamp();

        robot.oled.clear();
        robot.oled.displayText("finished");
    }

} // namespace Task2
