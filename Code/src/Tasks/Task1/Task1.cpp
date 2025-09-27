#include "Task1.h"
#include <Utils.h>

namespace Task1
{
    struct Box
    {
        int8_t x;
        int8_t y;

        Box() : x(0), y(0) {}
        Box(int8_t x_, int8_t y_) : x(x_), y(y_) {}
    };

    uint8_t x = 0;
    uint8_t y = 0;

    Box redBox{};
    Box greenBox{};
    Box blueBox{};

    Box REDSQUARE{6, 8};
    Box GREENSQUARE{4, 8};
    Box BLUESQUARE{2, 8};

    int8_t boxsFound = 0;
    Box uncoloredBoxs[4];

    void run(Robot &robot)
    {

        while (robot.junction.isTurn())
        {
            robot.followLine();
        }
        robot.turn90();
        int8_t xDir = 1;

        while (y < 7)
        {
            while (x < 7)
            {
                robot.followLine();
                if (robot.junction.isTurn())
                {
                    x += xDir;
                }
                if (robot.leftTof.readRange() < 10) // TODO: tune the value to accurtely detect left box
                {
                    _detectBox(robot, xDir, 1);
                }
                if (robot.grabberTof.readRange() < 10) // TODO: tune the value to accurtely detect left box
                {
                    _detectBox(robot, xDir, -1);
                }
                if (robot.frontTof.readRange() < 10) // TODO: tune the value to accurtely detect left box
                {
                    _detectBox(robot, xDir, 0);
                }
            }
        }
        xDir = -1 * xDir; // change thee direction of the x increment direction
        robot.turn(90 * (xDir));
        _go2(robot);
        robot.turn(90 * (xDir));
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

    void _addBox(uint8_t x, uint8_t y)
    {
        for (int8_t i = 0; i < boxsFound; i++)
        {
            if (uncoloredBoxs[i].x == x && uncoloredBoxs[i].y == y)
            {
                return;
            }
            boxsFound++;
            uncoloredBoxs[boxsFound].x = x;
            uncoloredBoxs[boxsFound].y = y;
        }
    }

    /**
     * _detectBox - Inspect and record a box adjacent to the robot.
     *
     * Parameters:
     *  - robot: Robot instance used for sensors and actuators.
     *  - xDir: relative x offset used to compute the box grid coordinate (typically +1 or -1).
     *  - directon: direction code where `0 = front`, `1 = left`, `-1 = right`.
     *
     * Behavior:
     *  - Turn to face the side indicated by xDir, use TOF sensors to detect an object.
     *  - If an uncolored box is detected, call _addBox; if colored, update the corresponding colored Box.
     *  - Pickup and return the box to the correct colored square (TODO).
     */
    void _detectBox(Robot &robot, int8_t xDir, int8_t directon)
    {
        if (directon != 0)
        {
            robot.turn(-90 * xDir * directon);
        }
        if (robot.frontTof.readRange() < robot.frontTopTof.readRange())
        {
            if (directon != 0)
            {
                _addBox(x, y + xDir * directon);
                robot.turn(90 * xDir * directon);
                robot.MotorR.goTillCM(5, 40);
                robot.MotorL.goTillCM(5, 40);
            }
            else
            {
                _addBox(x + xDir, y);
                // TODO: implement go around the box logic
            }
        }
        else
        {
            while (robot.frontTof.readRange() < 5) // TODO: tune the value to accurtely detect front box
            {
                robot.followLine();
            }
            int8_t tempX = 0;
            int8_t tempY = 0;
            // FIX: test the code for functionality
            if (directon != 0)
            {
                tempX = x;
                tempY = y + xDir * directon;
            }
            else
            {
                tempX = x + xDir;
                tempY = y;
            }
            switch (robot.boxColorSensor.getColor())
            {
            case COLOR_RED:
                redBox.x = tempX;
                redBox.y = tempY;
                break;
            case COLOR_BLUE:
                blueBox.x = tempX;
                blueBox.y = tempY;
                break;
            case COLOR_GREEN:
                greenBox.x = tempX;
                greenBox.y = tempY;
                break;

            default:
                break;
            }
            // TODO: implement code to pick box and return the box to its colored square.
        }
    }
}
