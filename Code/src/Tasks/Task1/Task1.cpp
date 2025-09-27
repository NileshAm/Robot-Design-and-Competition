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

    void _floodfill(Robot &robot, int8_t deltaX, int8_t deltaY)
    {
        int8_t stepX = (deltaX > 0) ? 1 : -1;
        int8_t stepY = (deltaY > 0) ? 1 : -1;

        while (deltaX != 0 || deltaY != 0)
        {
            while (deltaX != 0)
            {
                robot.followLine();
                if (robot.junction.isTurn())
                {
                    deltaX += stepX;
                }
                if (robot.frontTof.readRange() < 10)
                {
                    robot.turn(90 * stepY);
                    while (robot.junction.isTurn())
                    {
                        robot.followLine();
                    }
                    deltaY += stepY;
                    robot.turn(-90 * stepY);
                }
            }
            robot.turn(90 * stepY);
            while (deltaY != 0)
            {
                robot.followLine();
                if (robot.junction.isTurn())
                {
                    deltaY += stepY;
                }
                if (robot.frontTof.readRange() < 10)
                {
                    robot.turn(90 * stepX);
                    while (robot.junction.isTurn())
                    {
                        robot.followLine();
                    }
                    deltaX += stepX;
                    robot.turn(-90 * stepX);
                }
            }
        }
    }

    void _returnBox(Robot &robot, ColorName color)
    {
        int8_t deltaX = 0;
        int8_t deltaY = 0;
        switch (color)
        {
        case COLOR_RED:
            deltaX = REDSQUARE.x - x;
            deltaY = REDSQUARE.y - y;
            break;
        case COLOR_GREEN:
            deltaX = GREENSQUARE.x - x;
            deltaY = GREENSQUARE.y - y;
            break;
        case COLOR_BLUE:
            deltaX = BLUESQUARE.x - x;
            deltaY = BLUESQUARE.y - y;
            break;
        default:
            break;
        }
        _floodfill(robot, deltaX, deltaY);

        // TODO: drop the box

        robot.turn(180);
        _floodfill(robot, -deltaX, -deltaY);
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
            ColorName color = robot.boxColorSensor.getColor();
            switch (color)
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
            // TODO: implement code to pick box
            // turning and goiung back to the last junction
            robot.turn(180);
            while (robot.junction.isTurn())
            {
                robot.followLine();
            }
            // returning the box to the correct colored square and coming back to the initial position
            _returnBox(robot, color);
        }
    }

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
                robot.oled.displayText("X:" + String(x) + " Y:" + String(y), 0, 0, 1);
                robot.oled.display();
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

} // namespace Task1
