#include "Task5.h"
#include <Utils.h>

namespace Task5
{
    void run(Robot &robot)
    {
        while (!robot.junction.isAllWhite())
        {
            if (robot.junction.isLine())
            {
                robot.followLine();
            }
            if (robot.junction.isAllBlack())
            {
                robot.moveStraight();
            }
            if (robot.junction.isTurn())
            {
                while (!robot.junction.isLine())
                {
                    robot.turn90();
                }
                robot.followLine();
            }
        }
    }
}