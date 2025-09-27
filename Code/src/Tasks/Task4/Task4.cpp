#include "Task4.h"
#include <Utils.h>

namespace Task4
{
    void run(Robot &robot)
    {
        robot.turn90();
        while (robot.leftTof.readRange() > 5) // TODO: tune this value to change the wall gap
        {
            robot.followWall(); // FIX: change to followWall to single wall follow and implement single wall follow with 2 TOFs
        }
        robot.stop();
        robot.goTillCM(20); // TODO: tune this value to change the distance from wall to junction

        // TODO: lift the arm up

        robot.turn90();
        robot.goTillCM(2); // TODO: tune this value to change the forward movement step
        robot.stop();
        while (robot.leftTof.readRange() > 5) // TODO: tune this value to change the wall end detection
        {
            // TODO: lower the arm down
            ColorName color = robot.grabberSensor.getColor();
            ColorName selectionColor = COLOR_RED; // TODO: change this to read from EEPROM
            if (color == selectionColor)
            {
                // TODO: grab the ball
                // TODO: lift the arm up
                // TODO: release the ball
            }
            else
            {
                // TODO: lift the arm up
            }

            while (robot.MotorR.getTicks() < 50) // TODO: tune this value to change the forward movement step
            {
                robot.followWall(); // FIX: change to followWall to single wall follow and implement single wall follow with 2 TOFs
            }
        }
        robot.goTillCM(2); // TODO: tune this value to change the forward movement step
        robot.turn(-90);
        robot.goTillCM(20); // TODO: tune this value to change the distance from wall to junction
        robot.turn(-90);

        while (!robot.junction.isAllWhite())
        {
            robot.followWall(); // FIX: change to followWall to single wall follow and implement single wall follow with 2 TOFs
        }
        robot.turn90();

        while (robot.junction.isLine())
        {
            robot.followLine();
        }
        robot.turn90();
        while (robot.junction.isLine())
        {
            robot.followLine();
        }
    }
}