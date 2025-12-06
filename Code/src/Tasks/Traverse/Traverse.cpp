#include "Traverse.h"
#include <Utils.h>

namespace Traverse
{
    // --- Global State ---
    int8_t x = 0;
    int8_t y = 8; 
    // Facing: 0: Up (+Y?), no wait. 
    // User path: (0,8) -> (1,8) implies Right is +X.
    // (1,8) -> (1,7) implies Down is -Y.
    // So Y decreases going Down.
    // 0: Up (+Y), 1: Right (+X), 2: Down (-Y), 3: Left (-X)
    int facing = 1; // Start facing Right (+X) as we move (0,8)->(1,8)
    
    // --- Map Implementation ---
    enum BoxType {
        TYPE_NONE,
        TYPE_OBSTACLE,
        TYPE_BOX,
        TYPE_RED,
        TYPE_GREEN,
        TYPE_BLUE,
        TYPE_FILLED_DROP_ZONE
    };

    const int GRID_SIZE = 9;
    BoxType grid[GRID_SIZE][GRID_SIZE];

    void init_map() {
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                grid[i][j] = TYPE_NONE;
            }
        }
        // Add Targets (Top Line) similar to Task1 if needed, or just clear.
        // Task1 marks targets as filled drop zones.
        // For traversal, maybe we don't care, but good to have common structure.
        grid[2][0] = TYPE_FILLED_DROP_ZONE;
        grid[4][0] = TYPE_FILLED_DROP_ZONE;
        grid[6][0] = TYPE_FILLED_DROP_ZONE;
    }

    // --- Utils ---
    
    void updatePosition() {
        if (facing == 0) y++;
        else if (facing == 1) x++;
        else if (facing == 2) y--;
        else if (facing == 3) x--;
    }
    
    void turn(Robot& robot, int angle) {
        robot.turn(angle);
        delay(200); // Settle
        
        // Update facing
        // 90 is Right (+1), -90 is Left (-1)
        if (angle == 90) facing = (facing + 1) % 4;
        else if (angle == -90) facing = (facing - 1 + 4) % 4;
        else if (angle == 180) facing = (facing + 2) % 4;
    }
    
    void updateOLED(Robot& robot) {
        robot.oled.clear();
        robot.oled.displayText("Pos: " + String(x) + "," + String(y), 0, 0, 1);
    }

    void processDetectedObjects(Robot& robot, uint8_t detected) {
        if (detected == Robot::DETECT_NONE) return;

        bool front = (detected & Robot::DETECT_FRONT);
        bool left = (detected & Robot::DETECT_LEFT);
        bool right = (detected & Robot::DETECT_RIGHT);
        
        if (front || left || right) {
            robot.stop();
            robot.oled.clear();
            if (front) robot.oled.displayText("Front Object Detected", 0, 0, 1);
            if (left) robot.oled.displayText("Left Object Detected", 0, 10, 1);
            if (right) robot.oled.displayText("Right Object Detected", 0, 20, 1);
            
            // Stop forever (block task)
            while(true) {
                delay(100);
            }
        }
    }
    
    void run(Robot& robot)
    {
        // Initialize State
        x = 0;
        y = 8;
        facing = 1; // Start facing Right to go (0,8)->(1,8)
        init_map(); // Initialize Map
        
        robot.oled.clear();
        robot.oled.displayText("Traverse Start", 0, 0, 1);
        delay(1000);
        
        // 1. "go from (0,8) to (1,8) (move one cell forward)"
        uint8_t result = robot.goCellWithDetect();
        //processDetectedObjects(robot, result);
        robot.goCell();
        robot.goTillCM(5);
        robot.brake();
        updatePosition();
        updateOLED(robot);
        //delay(1000);
        turn(robot, -90);
        // 2. "then turn +90..then move one cell..now we are in (1,7) move like that to (1,0)"
        // Move 8 cells to reach (1,0)
        for(int i=0; i<2; i++) {
             result = robot.goCellWithDetect();
             processDetectedObjects(robot, result);
             robot.goCell();
            //  robot.goTillCM(5);
             robot.brake();
             updatePosition();
             updateOLED(robot);
             delay(500);
        }
        
        // //3. "the turn -90 ..then go 3 cells..now we are in (4,0)"
        // turn(robot, 90); // Face Right (1)
        // //delay(1000);

        // for(int i=0; i<3; i++) {
        //      result = robot.goCellWithDetect();
        //      processDetectedObjects(robot, result);
        //      robot.goCell();
        //      robot.goTillCM(5);
        //      robot.brake();
        //      updatePosition();
        //      updateOLED(robot);
        //      delay(1000);
        // }
        
        // // 4. "then turn go to (4,8)" -> Left Turn to face Up
        // turn(robot, -90); // Face Up (0)
        // delay(1000);
        
        // for(int i=0; i<4; i++) {
        //      result = robot.goCellWithDetect();
        //      processDetectedObjects(robot, result);
        //     robot.goCell();
        //      robot.goTillCM(5);
        //      robot.brake();
        //      updatePosition();
        //      updateOLED(robot);
        //      delay(1000);
        // }
        
        // // 5. "then turn 90 and go three cells to (7,8)"
        // turn(robot, 90); // Face Right (1)
        // delay(1000);
        // for(int i=0; i<3; i++) {
        //      result = robot.goCellWithDetect();
        //      processDetectedObjects(robot, result);
        //     robot.goCell();
        //      robot.goTillCM(5);
        //      robot.brake();
        //      updatePosition();
        //      updateOLED(robot);
        //      delay(1000);
        // }
        
        // // 6. "then turn 90 and go to (7,0)"
        // turn(robot, 90); // Face Down (2)
        // delay(1000);
        // for(int i=0; i<8; i++) {
        //      result = robot.goCellWithDetect();
        //      processDetectedObjects(robot, result);
        //     robot.goCell();
        //      robot.goTillCM(5);
        //      robot.brake();
        //      updatePosition();
        //      updateOLED(robot);
        //      delay(1000);
        // }

        robot.oled.clear();
        robot.oled.displayText("Traverse Done", 0, 0, 1);
        robot.stop();
        delay(2000);
    }

} // namespace Traverse
