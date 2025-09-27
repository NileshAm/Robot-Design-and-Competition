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
                    robot.turn(-90 * xDir);
                    if (robot.frontTof.readRange() < robot.frontTopTof.readRange()) 
                    {
                        _addBox(x, y + xDir);
                        robot.turn(90 * xDir);
                        robot.MotorR.goTillCM(5, 40);
                        robot.MotorL.goTillCM(5, 40);
                    }
                    else
                    {
                        while (robot.frontTof.readRange() < 5) // TODO: tune the value to accurtely detect front box
                        {
                            robot.followLine();
                        }
                        switch (robot.boxColorSensor.getColor())
                        {
                        case COLOR_RED:
                            redBox.x = x;
                            redBox.y = y + xDir;
                            break;
                        case COLOR_BLUE:
                            blueBox.x = x;
                            blueBox.y = y + xDir;
                            break;
                        case COLOR_GREEN:
                            greenBox.x = x;
                            greenBox.y = y + xDir;
                            break;

                        default:
                            break;
                        }
                        //TODO: implement code to pick box and return the box to its colored square.
                    }
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

    void _addBox(uint8_t x, uint8_t y){
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
}
