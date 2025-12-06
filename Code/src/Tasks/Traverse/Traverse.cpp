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
    
    //1,7 --turnright=2, turnleft=0, straight=1
    //4 - straight=3, turnright=0, turnleft=2

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

    
    void updateOLED(Robot& robot) {
        robot.oled.clear();
        robot.oled.displayText("Pos: " + String(x) + "," + String(y), 0, 0, 1);
    }

    void bypassLeft(Robot& robot) {
        robot.turnLeft();
        robot.goCell();
        updatePosition();
        robot.turnRight();
    }

    void bypassRight(Robot& robot) {
        robot.turnRight();
        robot.goCell();
        updatePosition();
        robot.turnLeft();
    }
    //FIX : Store position on map
    void storePos(Robot &robot) {
        int8_t tx = x;
        int8_t ty = y;

        if (facing == 0) ty++;
        else if (facing == 1) tx++;
        else if (facing == 2) ty--;
        else if (facing == 3) tx--;

        // Check bounds
        if (tx < 0 || tx >= GRID_SIZE || ty < 0 || ty >= GRID_SIZE) return;

        if (robot.detectObstacle()){
            grid[tx][ty] = TYPE_OBSTACLE;
            robot.oled.displayText("Obstacle at " + String(tx) + "," + String(ty), 0, 20, 1);
        }
        else{
            grid[tx][ty] = TYPE_BOX;
            robot.oled.displayText("Target at " + String(tx) + "," + String(ty), 0, 20, 1);
        }
    }
    
    void run(Robot& robot)
    {
        // Initialize State
        x = 0;
        y = 0;
        facing = 1; // Start facing Right to go (0,8)->(1,8)
        init_map(); // Initialize Map
        
        robot.oled.clear();
        robot.oled.displayText("Traverse Start", 0, 0, 1);
        delay(1000);

        while (robot.junction.isAllBlack())
        {
            robot.moveStraight();
        }
        robot.turnRight();
        robot.goCell();
        x = 1;
        y = 1;

        for (int row = 0; row < 9; row = row + 3)
        {
            for (int col = 0; col < 9; col++)
            {
                robot.goCell();

                // updatePosition();
                // updateOLED(robot);
                
                if (robot.detectFrontBox()){
                    robot.oled.displayText("Front Box", 0, 10, 1);
                    storePos(robot);
                }
                if (robot.detectLeftBox()){
                    robot.oled.displayText("Left Box", 0, 10, 1);
                    robot.turnLeft();
                    storePos(robot); //TODO : update facing direction
                    robot.turnRight(); 
                }
                //facing directions
                //1,7 --turnright=2, turnleft=0, straight=1
                //4 - straight=3, turnright=0, turnleft=2
                if (robot.detectRightBox()){
                    robot.oled.displayText("Right Box", 0, 10, 1);
                    robot.turnRight();
                    storePos(robot);
                    robot.turnLeft();
                }
            }
            if (row%2==0){
                robot.turnRight();
                robot.goCell(2);
                y = y + 2; 
                robot.turnRight();
            }
            else{
                robot.turnLeft();
                robot.goCell(2);
                y = y + 2; 
                robot.turnLeft();
            }

        }
        

        robot.oled.clear();
        robot.oled.displayText("Traverse Done", 0, 0, 1);
        robot.brake();
        delay(2000);
    }

} // namespace Traverse
