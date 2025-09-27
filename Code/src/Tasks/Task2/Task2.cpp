#include "Task2.h"
#include <Utils.h>

namespace Task2
{
    void run(Robot &robot)
    {
        while (!robot.junction.isAllWhite())
        {
            if (robot.junction.isLine())
            {
                robot.followLine();
            }
            else
            {
                robot.moveStraight();
            }
        }
        robot.stop();

        // TODO: add functionality to starighten the robot if it is not aligned properly

        robot.goTillCM(50);

        // FIX: change the robot class to implement going back straight for a certain distance
        robot.MotorL.resetTicks();
        robot.MotorR.resetTicks();
        while (robot.MotorL.getTicks() > -100)
        {
            robot.MotorL.setSpeed(-40);
            robot.MotorR.setSpeed(-40);
            delay(1);
        }
        robot.stop();
        //end of fix
        
        robot.turn(180);
        while (robot.junction.isAllWhite())
        {
            robot.moveStraight();
        }

        while (!robot.junction.isAllWhite())
        {
            if (robot.junction.isLine())
            {
                robot.followLine();
            }
            else
            {
                robot.moveStraight();
            }
        }

        // FIX: change the robot class to implement going back straight for a certain distance
        robot.MotorL.resetTicks();
        robot.MotorR.resetTicks();
        while (robot.MotorL.getTicks() > -100)
        {
            robot.MotorL.setSpeed(-40);
            robot.MotorR.setSpeed(-40);
            delay(1);
        }
        //end of fix

        robot.stop();
        robot.turn(90);

        while (!robot.junction.isAllWhite())
        {
            robot.moveStraight();
        }
    }
} // namespace Task2