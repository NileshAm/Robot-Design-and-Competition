#include "Task3.h"
#include <Utils.h>

namespace Task3 {
    void run(Robot& robot , Junction& junction) {
        int acc = 0;
        while (!junction.isLine()){
            robot.moveStraight();
        }
        
        while (junction.isTurn())
        {
            robot.followLine();
        }
        robot.turn90();
        while (junction.isAllWhite())
        {
            robot.followLine();
        }
        

        long ticksPer0 = 200; // TODO: tune this value
        while (!junction.isLine())
        {
            robot.MotorL.resetTicks();
            robot.MotorR.resetTicks();
            while (junction.isAllWhite())
            {
                robot.moveStraight(); // TODO: make able to set speed
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
            
            while (junction.isAllBlack())
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