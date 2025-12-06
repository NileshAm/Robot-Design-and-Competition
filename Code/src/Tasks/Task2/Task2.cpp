#include "Task2.h"

namespace Task2
{
    void run(Robot &robot)
    {
        robot.oled.displayText("Task2 Start", 0, 0, 1);
        robot.oled.display();
        while (!robot.junction.isAllWhite())
        {
            if (robot.junction.isLine())
            {
                robot.followLine(25);
                robot.MotorL.resetTicks();
                robot.MotorR.resetTicks();
            }
            else
            {
                if ((robot.MotorL.getTicks() + robot.MotorR.getTicks()) / 2 < 375)
                {
                    robot.moveStraight(25);
                }
                else
                {
                    robot.MotorL.resetTicks();
                    robot.MotorR.resetTicks();
                    while (labs(robot.MotorL.getTicks() + robot.MotorR.getTicks()) / 2 < 350)
                    {
                        robot.moveStraight(-25);
                    }
                    robot.turn(-25);
                }
            }
        }
        robot.brake();
    }

} // namespace Task2
