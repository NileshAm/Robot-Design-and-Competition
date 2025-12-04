#include "Task3.h"
#include <Utils.h>

namespace Task3 {
    void run(Robot& robot) {
        int acc = 0;
        while (!robot.junction.isLine()){
            robot.moveStraight();
        }
        
        while (robot.junction.isTurn())
        {
            robot.followLine();
        }
        robot.turn90();
        while (robot.junction.isAllWhite())
        {
            robot.followLine();
        }
        

        long ticksPer0 = 200; // TODO: tune this value
        while (!robot.junction.isLine())
        {
            robot.MotorL.resetTicks();
            robot.MotorR.resetTicks();
            while (robot.junction.isAllWhite())
            {
                robot.moveStraight(30);
            }
            long ticks = (robot.MotorL.getTicks() + robot.MotorR.getTicks()) / 2;
            if (ticks < ticksPer0)
            {
                acc = pushBitMSB(acc, 0);
            }
            else
            {
                acc = pushBitMSB(acc, 1);
            }
            
            while (robot.junction.isAllBlack())
            {
                robot.moveStraight();
            }
            
        }
        robot.stop();
        Serial.print("Decimal: ");
        Serial.println(acc);
        // TODO: push to EEPROM

    }
}