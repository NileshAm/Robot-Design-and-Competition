#include "Task1.h"
#include <Utils.h>

namespace Task1
{
    uint8_t x = 0;
    uint8_t y = 0;
    void run(Robot &robot)
    {

        while (robot.junction.isTurn())
        {
            robot.followLine();
        }
        robot.turn90();
        uint8_t xDir = 1;

        while (y < 7)
        {
            while (x < 7)
            {
                robot.followLine();
                if (robot.junction.isTurn())
                {
                    x += xDir;
                }
            }
            xDir = -1 * xDir; // change thee direction of the x increment direction
            robot.turn(90 * (xDir));
            _go2(robot);
            robot.turn(90 * (xDir));
        }
    }

    void _go2(Robot &robot)
    {
        while (robot.junction.isTurn() && y == 1)
        {
            if (robot.junction.isTurn())
            {
                y += 1;
            }
        }
        y += 1;
    }
}