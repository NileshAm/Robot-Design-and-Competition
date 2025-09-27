#include "Task7.h"
#include <Utils.h>

namespace Task7
{
    void run(Robot &robot)
    {
        int8_t count = 0;
        while (true)
        {
            if (count == 1)
            {
                break;
            }
            if (robot.frontTof.readRange() < 5)
            {
                count++;
            }
            delay(100);
        }
        delay(500); // TODO: adjest to delay gate to open for robot to pass
        robot.moveStraight(30);

        while (robot.leftTof.readRange() < 15 && robot.grabberTof.readRange() < 15) // TODO: adjust distance to detect end of wall following
        {
            robot.followWall(); // FIX: change the method to use left and right tof to follow tghecenter line of the wall
        }

        while (robot.junction.isAllBlack())
        {
            robot.moveStraight();
        }

        robot.goTillCM(10); // TODO: to set the position of the robot in the square
        robot.turn90();
        robot.goTillCM(20); // TODO: to set the position of the robot in the square

        // TODO: implement the code to shoot the balls
    }
}