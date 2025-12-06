///don't use this untill front color sensor is placed

#include "Task1_FrontColor.h"
#include <Utils.h>
#include <Grabber.h>
#include <ServoMotor.h>
#include <CurrentSensor.h>

namespace Task1Front
{
    // --- Constants ---
    const int GRID_SIZE = 9;
    const int MAX_QUEUE = 100;

    // --- Structures ---
    struct Point {
        int8_t x;
        int8_t y;

        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }
        bool operator!=(const Point& other) const {
            return !(*this == other);
        }
    };

    enum BoxType {
        TYPE_NONE,
        TYPE_OBSTACLE,
        TYPE_BOX,
        TYPE_RED,
        TYPE_GREEN,
        TYPE_BLUE,
        TYPE_FILLED_DROP_ZONE
    };

    // --- Simple Queue Implementation ---
    struct PointQueue {
        Point items[MAX_QUEUE];
        int head = 0;
        int tail = 0;
        int count = 0;

        void push(Point p) {
            if (count < MAX_QUEUE) {
                items[tail] = p;
                tail = (tail + 1) % MAX_QUEUE;
                count++;
            }
        }

        Point pop() {
            if (count > 0) {
                Point p = items[head];
                head = (head + 1) % MAX_QUEUE;
                count--;
                return p;
            }
            return {-1, -1};
        }

        bool isEmpty() {
            return count == 0;
        }

        void clear() {
            head = 0;
            tail = 0;
            count = 0;
        }
    };

    // --- Global State ---
    int8_t x = 0;
    int8_t y = 8; // Start Bottom Left
    int8_t direction = 1; // 1 for Right, -1 for Left
    int facing = 0; // 0: Up (-Y), 1: Right (+X), 2: Down (+Y), 3: Left (-X)
    
    const int scan_rows[] = {7, 4, 1};
    int current_row_idx = 0;

    PointQueue move_queue;
    PointQueue path_queue;
    
    BoxType carrying = TYPE_NONE;
    Point target_box_pos = {-1, -1};

    enum State {
        STATE_INIT,
        STATE_SCANNING,
        STATE_COLLECTING,
        STATE_FINISHED
    };
    State state = STATE_INIT;

    enum CollectionPhase {
        PHASE_IDLE,
        PHASE_MOVING_TO_BOX,
        PHASE_MOVING_TO_DROP
    };
    CollectionPhase collection_phase = PHASE_IDLE;

    // Grid Map (2D Array)
    BoxType grid[GRID_SIZE][GRID_SIZE];
    Point targets[3]; // Red, Green, Blue

    void init_map() {
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                grid[i][j] = TYPE_NONE;
            }
        }
        // Add Targets (Top Line)
        targets[0] = {2, 0}; // RED
        targets[1] = {4, 0}; // GREEN
        targets[2] = {6, 0}; // BLUE
        
        // Mark targets as obstacles so we don't walk through them
        grid[2][0] = TYPE_FILLED_DROP_ZONE;
        grid[4][0] = TYPE_FILLED_DROP_ZONE;
        grid[6][0] = TYPE_FILLED_DROP_ZONE;
    }

    bool is_obstacle(int8_t cx, int8_t cy) {
        if (cx < 0 || cx >= GRID_SIZE || cy < 0 || cy >= GRID_SIZE) return true;
        return grid[cx][cy] == TYPE_OBSTACLE || grid[cx][cy] == TYPE_FILLED_DROP_ZONE;
    }

    // --- Helper Functions ---

    bool is_valid_pos(int8_t cx, int8_t cy) {
        return cx >= 0 && cx < GRID_SIZE && cy >= 0 && cy < GRID_SIZE;
    }

    int abs_val(int val) {
        return (val < 0) ? -val : val;
    }

    // --- Sensing Helpers ---

    BoxType classifyObject(Robot& robot) {
        // Assumes robot is facing the object
        // Use robot.detectObstacle() which checks both Front and FrontTop TOF
        
        if (robot.detectObstacle()) {
            return TYPE_OBSTACLE; // Tall object
        } else if (robot.detectFrontBox()) {
            return TYPE_BOX; // Short object (Front detected, but not Obstacle)
        }
        
        return TYPE_NONE;
    }

    ColorName approachAndScan(Robot& robot) {
        // Pre-condition: Robot is facing the object (Box)
        // Action: Move close, Scan with Front Color Sensor, Return.
        // NO TURNING NEEDED
        
        robot.oled.displayText("Approaching...", 0, 20, 1);
        
        // 1. Move Forward until close
        // We want to be ~40-50mm away for the front color sensor
        // Safety timeout
        unsigned long t0 = millis();
        // Move closer than before (e.g. < 50mm)
        while (robot.frontTof.readRange() > 50 && millis() - t0 < 2000) {
            robot.moveStraight(15); // Slow approach
            delay(10);
        }
        robot.stop();
        
        // 2. Scan Color (Front Sensor)
        delay(200); // Stabilize
        ColorName color = robot.boxColorSensor.getColor();
        
        // 3. Move Backward to original position
        // Move back until we hit the junction (all white line)
        robot.moveStraight(-15);
        while (!robot.junction.isAllWhite()) {
            robot.ir.updateSensors();
            delay(1);
        }
        robot.stop();
        
        return color;
    }

    // --- Navigation ---

    void turn_to_face(Robot& robot, int target_facing) {
        int diff = target_facing - facing;
        if (diff == 0) return;
        
        // Normalize diff to -1, 0, 1, 2 (180)
        if (diff == 3) diff = -1; // Left turn (e.g. 0 -> 3)
        if (diff == -3) diff = 1; // Right turn (e.g. 3 -> 0)
        
        if (diff == 1) {
            robot.turn(90);
        } else if (diff == -1) {
            robot.turn(-90);
        } else if (diff == 2 || diff == -2) {
            robot.turn(180); 
        }
        
        facing = target_facing;
    }

    void execute_move(Robot& robot, Point next) {
        int dx = next.x - x;
        int dy = next.y - y;
        
        int target_facing = -1;
        if (dy == -1) target_facing = 0; // Up
        else if (dx == 1) target_facing = 1; // Right
        else if (dy == 1) target_facing = 2; // Down
        else if (dx == -1) target_facing = 3; // Left
        
        if (target_facing != -1) {
            turn_to_face(robot, target_facing);
            robot.goCell(); // Move one cell forward
            x = next.x;
            y = next.y;
        }
    }

    // --- Sensing ---

    void scan_and_report_local(Robot& robot) {
        auto get_rel_pos = [&](int fwd, int right) -> Point {
            int abs_dx = 0, abs_dy = 0;
            if (facing == 0) { abs_dy -= fwd; abs_dx += right; }      // Up
            else if (facing == 1) { abs_dx += fwd; abs_dy += right; } // Right
            else if (facing == 2) { abs_dy += fwd; abs_dx -= right; } // Down
            else if (facing == 3) { abs_dx -= fwd; abs_dy -= right; } // Left
            return {static_cast<int8_t>(x + abs_dx), static_cast<int8_t>(y + abs_dy)};
        };

        // 1. Check Front
        if (robot.detectFrontBox()) {
            Point p = get_rel_pos(1, 0);
            if (is_valid_pos(p.x, p.y) && (grid[p.x][p.y] == TYPE_NONE || grid[p.x][p.y] == TYPE_OBSTACLE)) {
                BoxType type = classifyObject(robot);
                if (type == TYPE_BOX) {
                    // It's a box, we need to scan color.
                    // Approach and Scan
                    ColorName color = approachAndScan(robot);
                    if (color == COLOR_RED) grid[p.x][p.y] = TYPE_RED;
                    else if (color == COLOR_GREEN) grid[p.x][p.y] = TYPE_GREEN;
                    else if (color == COLOR_BLUE) grid[p.x][p.y] = TYPE_BLUE;
                    else grid[p.x][p.y] = TYPE_OBSTACLE; // Failed to ID color
                } else {
                    grid[p.x][p.y] = TYPE_OBSTACLE;
                }
            }
        }
        
        // 2. Check Right
        if (robot.detectRightBox()) {
            Point p = get_rel_pos(0, 1);
            if (is_valid_pos(p.x, p.y) && (grid[p.x][p.y] == TYPE_NONE || grid[p.x][p.y] == TYPE_OBSTACLE)) {
                // Turn to Face
                robot.turn(90);
                BoxType type = classifyObject(robot);
                if (type == TYPE_BOX) {
                    ColorName color = approachAndScan(robot);
                    if (color == COLOR_RED) grid[p.x][p.y] = TYPE_RED;
                    else if (color == COLOR_GREEN) grid[p.x][p.y] = TYPE_GREEN;
                    else if (color == COLOR_BLUE) grid[p.x][p.y] = TYPE_BLUE;
                    else grid[p.x][p.y] = TYPE_OBSTACLE;
                } else {
                    grid[p.x][p.y] = TYPE_OBSTACLE;
                }
                // Return
                robot.turn(-90);
            }
        }

        // 3. Check Left
        if (robot.detectLeftBox()) {
            Point p = get_rel_pos(0, -1);
            if (is_valid_pos(p.x, p.y) && (grid[p.x][p.y] == TYPE_NONE || grid[p.x][p.y] == TYPE_OBSTACLE)) {
                // Turn to Face
                robot.turn(-90);
                BoxType type = classifyObject(robot);
                if (type == TYPE_BOX) {
                    ColorName color = approachAndScan(robot);
                    if (color == COLOR_RED) grid[p.x][p.y] = TYPE_RED;
                    else if (color == COLOR_GREEN) grid[p.x][p.y] = TYPE_GREEN;
                    else if (color == COLOR_BLUE) grid[p.x][p.y] = TYPE_BLUE;
                    else grid[p.x][p.y] = TYPE_OBSTACLE;
                } else {
                    grid[p.x][p.y] = TYPE_OBSTACLE;
                }
                // Return
                robot.turn(90);
            }
        }
    }

    // --- Pathfinding ---

    bool bfs_path(Point start, Point end, PointQueue& outPath) {
        PointQueue q;
        bool visited[GRID_SIZE][GRID_SIZE];
        Point parent[GRID_SIZE][GRID_SIZE];
        
        for(int i=0; i<GRID_SIZE; i++) 
            for(int j=0; j<GRID_SIZE; j++) 
                visited[i][j] = false;

        q.push(start);
        visited[start.x][start.y] = true;
        parent[start.x][start.y] = {-1, -1};
        
        bool found = false;
        const int dx[] = {0, 0, -1, 1};
        const int dy[] = {-1, 1, 0, 0};

        while (!q.isEmpty()) {
            Point curr = q.pop();
            
            if (curr == end) {
                found = true;
                break;
            }
            
            for (int i = 0; i < 4; i++) {
                int8_t nx = curr.x + dx[i];
                int8_t ny = curr.y + dy[i];
                
                if (is_valid_pos(nx, ny) && !visited[nx][ny]) {
                    // Check obstacle (allow if it's the destination)
                    if (is_obstacle(nx, ny) && (nx != end.x || ny != end.y)) {
                        continue;
                    }
                    
                    visited[nx][ny] = true;
                    parent[nx][ny] = curr;
                    q.push({nx, ny});
                }
            }
        }

        if (found) {
            // Reconstruct path
            // We need to store it in reverse then flip? 
            // Or just store in a temporary array.
            Point tempPath[MAX_QUEUE];
            int pathLen = 0;
            Point curr = end;
            
            while (curr != start) {
                tempPath[pathLen++] = curr;
                curr = parent[curr.x][curr.y];
            }
            
            // Push to outPath in correct order (start -> end)
            // Note: BFS path usually excludes start.
            outPath.clear();
            for (int i = pathLen - 1; i >= 0; i--) {
                outPath.push(tempPath[i]);
            }
            return true;
        }
        return false;
    }

    // --- Logic ---

    bool calculate_dynamic_bypass(int8_t blocked_x, int8_t blocked_y) {
        int8_t candidate_lanes[2];
        int lane_count = 0;
        
        if (is_valid_pos(x, y - 1) && !is_obstacle(x, y - 1)) candidate_lanes[lane_count++] = y - 1;
        if (is_valid_pos(x, y + 1) && !is_obstacle(x, y + 1)) candidate_lanes[lane_count++] = y + 1;
        
        for (int k = 0; k < lane_count; k++) {
            int8_t bypass_y = candidate_lanes[k];
            Point temp_path[GRID_SIZE * 2];
            int path_len = 0;
            bool lane_success = true;
            bool re_entry_found = false;
            
            temp_path[path_len++] = {x, bypass_y};
            
            for (int i = 1; i <= GRID_SIZE; i++) {
                int8_t next_check_x = x + (i * direction);
                
                if (!is_valid_pos(next_check_x, bypass_y)) {
                    re_entry_found = true;
                    break;
                }
                
                if (is_obstacle(next_check_x, bypass_y)) {
                    lane_success = false;
                    break;
                }
                
                temp_path[path_len++] = {next_check_x, bypass_y};
                
                if (is_valid_pos(next_check_x, y) && !is_obstacle(next_check_x, y)) {
                    temp_path[path_len++] = {next_check_x, y};
                    re_entry_found = true;
                    break;
                }
            }
            
            if (lane_success && re_entry_found) {
                for(int i=0; i<path_len; i++) {
                    move_queue.push(temp_path[i]);
                }
                return true;
            }
        }
        return false;
    }

    void queue_row_change() {
        current_row_idx++;
        if (current_row_idx >= 3) { 
            state = STATE_COLLECTING;
            return;
        }
        
        int8_t target_row = scan_rows[current_row_idx];
        int8_t temp_y = y;
        int8_t step = (target_row < y) ? -1 : 1;
        
        while (temp_y != target_row) {
            temp_y += step;
            move_queue.push({x, temp_y});
        }
        direction *= -1;
    }

    Point determine_next_move() {
        if (!move_queue.isEmpty()) {
            return move_queue.pop();
        }
        
        if (state == STATE_INIT) {
            int8_t start_row = scan_rows[0];
            if (y > start_row) return {x, static_cast<int8_t>(y - 1)};
            else {
                state = STATE_SCANNING;
            }
        }
        
        if (state == STATE_SCANNING) {
            int8_t next_x = x + direction;
            
            if (!is_valid_pos(next_x, y)) {
                queue_row_change();
                if (!move_queue.isEmpty()) {
                    return move_queue.pop();
                }
                return {-1, -1};
            }
            
            if (is_obstacle(next_x, y)) {
                bool success = calculate_dynamic_bypass(next_x, y);
                if (success && !move_queue.isEmpty()) {
                    return move_queue.pop();
                } else {
                    return {-1, -1}; 
                }
            } else {
                return {next_x, y};
            }
        }
        
        if (state == STATE_COLLECTING) {
            if (!path_queue.isEmpty()) {
                return path_queue.pop();
            }
            
            if (collection_phase == PHASE_MOVING_TO_BOX) {
                if (target_box_pos.x != -1 && x == target_box_pos.x && y == target_box_pos.y) {
                    return {-1, -1}; 
                }
            } else if (collection_phase == PHASE_MOVING_TO_DROP) {
                return {-1, -1}; 
            }
            
            if (carrying != TYPE_NONE) {
                Point target_pos = {-1, -1};
                // Map carrying type to target index
                if (carrying == TYPE_RED) target_pos = targets[0];
                else if (carrying == TYPE_GREEN) target_pos = targets[1];
                else if (carrying == TYPE_BLUE) target_pos = targets[2];
                
                if (target_pos.x != -1) {
                    Point dest = {target_pos.x, static_cast<int8_t>(target_pos.y + 1)};
                    if (bfs_path({x, y}, dest, path_queue)) {
                        collection_phase = PHASE_MOVING_TO_DROP;
                        return path_queue.pop();
                    }
                }
            } else {
                Point nearest_box = {-1, -1};
                int min_dist = 999;
                
                for (int i=0; i<GRID_SIZE; i++) {
                    for (int j=0; j<GRID_SIZE; j++) {
                        BoxType type = grid[i][j];
                        if (type == TYPE_RED || type == TYPE_GREEN || type == TYPE_BLUE || type == TYPE_OBSTACLE) {
                            int dist = abs_val(x - i) + abs_val(y - j);
                            if (dist < min_dist) {
                                min_dist = dist;
                                nearest_box = {static_cast<int8_t>(i), static_cast<int8_t>(j)};
                            }
                        }
                    }
                }
                
                if (nearest_box.x != -1) {
                    if (bfs_path({x, y}, nearest_box, path_queue)) {
                        collection_phase = PHASE_MOVING_TO_BOX;
                        target_box_pos = nearest_box;
                        return path_queue.pop();
                    }
                } else {
                    state = STATE_FINISHED;
                }
            }
        }
        
        return {-1, -1};
    }

    void run(Robot& robot)
    {
        // --- Grabber is now part of Robot ---
        // robot.grabber.init(); // Already initialized in main
        
        // Initialize State
        init_map();
        x = 0;
        y = 8;
        direction = 1;
        facing = 0; 
        
        robot.oled.clear();
        robot.oled.displayText("Task1 Front Start", 0, 0, 1);
        
        while (state != STATE_FINISHED)
        {
            if (robot.isInterrupted()) return;
            
            scan_and_report_local(robot);
            Point next = determine_next_move();
            
            if (next.x != -1) {
                execute_move(robot, next);
            } else {
                if (state == STATE_COLLECTING) {
                    if (collection_phase == PHASE_MOVING_TO_BOX && target_box_pos.x != -1 && x == target_box_pos.x && y == target_box_pos.y) {
                        // Identify Color
                        ColorName color = robot.grabberSensor.getColor(); 
                        BoxType type = TYPE_OBSTACLE;
                        if (color == COLOR_RED) type = TYPE_RED;
                        else if (color == COLOR_GREEN) type = TYPE_GREEN;
                        else if (color == COLOR_BLUE) type = TYPE_BLUE;
                        
                        if (type != TYPE_OBSTACLE) {
                            robot.oled.displayText("Grabbing...", 0, 10, 1);
                            robot.grabber.grab();
                            robot.grabber.lift();
                            carrying = type;
                            grid[x][y] = TYPE_NONE; // Picked up
                        } else {
                            grid[x][y] = TYPE_OBSTACLE; 
                            target_box_pos = {-1, -1};
                            collection_phase = PHASE_IDLE;
                        }
                        
                        collection_phase = PHASE_IDLE;
                        target_box_pos = {-1, -1};
                        
                    } else if (collection_phase == PHASE_MOVING_TO_DROP) {
                         robot.oled.displayText("Dropping...", 0, 10, 1);
                         robot.grabber.release();
                         robot.grabber.reset();
                         carrying = TYPE_NONE;
                         collection_phase = PHASE_IDLE;
                    }
                }
            }
            
            robot.oled.clear();
            robot.oled.displayText("State: " + String(state), 0, 0, 1);
            robot.oled.displayText("Pos: " + String(x) + "," + String(y), 0, 10, 1);
        }
        
        robot.oled.clear();
        robot.oled.displayText("Task Finished", 0, 0, 1);
        robot.stop();
    }

} // namespace Task1Front
