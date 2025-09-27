#include "Task6.h"
#include <Utils.h>

namespace Task6
{
    void run(Robot &robot)
    {
        robot.goTillCM(10);
        while (robot.frontTof.readRange() > 10)
        {
            robot.followWall(); // TODO: change the wall folllow function to use both left right tof to follow the centr line
        }
        robot.turn90();
        robot.MotorL.resetTicks();
        robot.MotorR.resetTicks();
        while ((robot.MotorL.getTicks() + robot.MotorR.getTicks()) / 2 < 300) // TODO: tune this value to change moving forward distance after turn
        {
            robot.followWall(); // TODO: change the wall folllow function to use both left right tof to follow the centr line
        }
    }

}