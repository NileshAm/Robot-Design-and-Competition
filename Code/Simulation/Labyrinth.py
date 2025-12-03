import pygame
import sys
import random

# --- Constants ---
SCREEN_WIDTH = 600
SCREEN_HEIGHT = 650
GRID_SIZE = 9  # Indices 0 to 8
CELL_SIZE = 55 
MARGIN = 50

# Colors
COLOR_BG = (10, 10, 10)       
COLOR_GRID = (80, 80, 80)     
COLOR_OBS = (255, 255, 255)   
COLOR_ROBOT = (255, 255, 0)   
COLOR_SCAN_LINE = (0, 255, 255) 
COLOR_BUTTON = (50, 100, 200)
COLOR_BUTTON_HOVER = (70, 120, 220)

# --- Classes ---

class GridMap:
    def __init__(self):
        self.obstacles = {} 
        self.targets = {}
        self.known_obstacles = set() 
        self.recorded_items = [] 
        
        # Start with a random map
        self.generate_random_map()
        
        # Add Targets (Top Line)
        self.add_target(2, 0, (255, 0, 0))   # Red
        self.add_target(4, 0, (0, 255, 0))   # Green
        self.add_target(6, 0, (0, 0, 255))   # Blue

    def generate_random_map(self):
        self.obstacles = {}
        
        # Define the 6 items: 2 colored boxes, 4 obstacles
        # Colors: 'r', 'g', 'b'. We need 2 random colors.
        colors = ['r', 'g', 'b']
        chosen_colors = random.sample(colors, 2)
        items = chosen_colors + ['obstacle'] * 4
        random.shuffle(items)
        
        count = 0
        while count < len(items):
            # Range 1 to 7 ensures boxes are NEVER on the boundary lines (0 or 8)
            rx = random.randint(1, 7)
            ry = random.randint(1, 7) 
            
            # Safety checks (don't spawn on top of targets or robot start)
            if ry == 0: continue  
            if ry == 8 and rx == 0: continue 
            
            if (rx, ry) not in self.obstacles:
                self.add_obstacle(rx, ry, items[count])
                count += 1

    def add_obstacle(self, x, y, type='obstacle'):
        self.obstacles[(x, y)] = type

    def remove_obstacle(self, x, y):
        if (x, y) in self.obstacles:
            del self.obstacles[(x, y)]
        if (x, y) in self.known_obstacles:
            self.known_obstacles.remove((x, y))

    def add_target(self, x, y, color):
        self.targets[(x, y)] = color

    def is_obstacle(self, x, y):
        return (x, y) in self.obstacles

    def get_target(self, x, y):
        return self.targets.get((x, y))

class Robot:
    def __init__(self, grid_map):
        self.grid_map = grid_map
        self.x = 0
        self.y = 8 # Start Bottom Left
        
        self.state = "INIT"  
        self.scan_rows = [7, 4, 1] 
        self.current_row_idx = 0
        self.direction = 1 
        
        self.move_queue = []
        
        self.pixel_x = self.grid_to_pixel(self.x)
        self.pixel_y = self.grid_to_pixel(self.y)
        self.speed = 5
        self.scan_beams = [] 
        
        self.found_boxes = {} # (x, y) -> type
        self.carrying = None # type of box currently carrying
        self.path_queue = [] # Queue for BFS path 
        
        self.collection_phase = "IDLE" # IDLE, MOVING_TO_BOX, MOVING_TO_DROP
        self.target_box_pos = None # (x, y) of the box we are going for 

    def grid_to_pixel(self, val):
        return MARGIN + val * CELL_SIZE

    def is_valid_pos(self, x, y):
        return 0 <= x < GRID_SIZE and 0 <= y < GRID_SIZE

    def update(self):
        target_px = self.grid_to_pixel(self.x)
        target_py = self.grid_to_pixel(self.y)
        
        # Movement Animation Logic
        if self.pixel_x < target_px: self.pixel_x = min(self.pixel_x + self.speed, target_px)
        elif self.pixel_x > target_px: self.pixel_x = max(self.pixel_x - self.speed, target_px)
        
        if self.pixel_y < target_py: self.pixel_y = min(self.pixel_y + self.speed, target_py)
        elif self.pixel_y > target_py: self.pixel_y = max(self.pixel_y - self.speed, target_py)

        # When animation finishes, Think next move
        if self.pixel_x == target_px and self.pixel_y == target_py:
            self.think() 

    def think(self):
        # 1. Scan current location
        self.scan_and_report_local()

        # 2. Get Next Move
        next_pos = self.determine_next_move()

        # 3. Apply Move (only if valid)
        if next_pos:
            nx, ny = next_pos
            if self.is_valid_pos(nx, ny):
                self.x, self.y = nx, ny
            else:
                print(f"CRITICAL ERROR: Attempted to move out of bounds to ({nx}, {ny})")

    def determine_next_move(self):
        # Priority 1: Execute Queued Moves
        if len(self.move_queue) > 0:
            return self.move_queue.pop(0)

        # Priority 2: State Machine Logic
        if self.state == "INIT":
            start_row = self.scan_rows[0]
            if self.y > start_row: return (self.x, self.y - 1)
            else: 
                self.state = "SCANNING"
        
        if self.state == "SCANNING":
            next_x = self.x + self.direction
            
            # A. Check Map Boundaries (End of Row)
            if not self.is_valid_pos(next_x, self.y):
                self.queue_row_change()
                if self.move_queue: return self.move_queue.pop(0)
                else: return None

            # B. Check Obstacles
            if self.grid_map.is_obstacle(next_x, self.y):
                success = self.calculate_dynamic_bypass(next_x, self.y)
                if success and self.move_queue:
                    return self.move_queue.pop(0)
                else:
                    return None
            else:
                return (next_x, self.y)

        if self.state == "COLLECTING":
            # If we have a path, follow it
            if self.path_queue:
                return self.path_queue.pop(0)
            
            # If we just finished a path, check what we need to do
            if self.collection_phase == "MOVING_TO_BOX":
                # We arrived at the box (or adjacent to it)
                # Actually, our BFS target was the box position itself?
                # Let's check if we are AT the target box
                if self.target_box_pos and (self.x, self.y) == self.target_box_pos:
                    # PICK UP
                    if self.target_box_pos in self.found_boxes:
                        self.carrying = self.found_boxes.pop(self.target_box_pos)
                        self.grid_map.remove_obstacle(self.target_box_pos[0], self.target_box_pos[1])
                    self.collection_phase = "IDLE"
                    self.target_box_pos = None
                    return None # Wait one cycle to think again

            elif self.collection_phase == "MOVING_TO_DROP":
                # We arrived at the drop zone (or adjacent)
                # Our BFS target was (target_x, target_y + 1)
                # DROP
                
                # Identify the actual drop zone position (target_x, target_y)
                # We are at (x, y), drop zone is at (x, y-1)
                drop_zone_pos = (self.x, self.y - 1)
                
                # Mark it as an obstacle so we don't try to go there (or if it matters for pathing)
                self.grid_map.add_obstacle(drop_zone_pos[0], drop_zone_pos[1], 'filled_drop_zone')
                self.grid_map.known_obstacles.add(drop_zone_pos)
                
                self.carrying = None
                self.collection_phase = "IDLE"
                return None

            # If IDLE, plan next move
            if self.carrying:
                # Find target for carrying color
                target_pos = None
                for pos, color in self.grid_map.targets.items():
                    # Map color tuple to char code
                    color_code = None
                    if color == (255, 0, 0): color_code = 'r'
                    elif color == (0, 255, 0): color_code = 'g'
                    elif color == (0, 0, 255): color_code = 'b'
                    
                    if color_code == self.carrying:
                        target_pos = pos
                        break
                
                if target_pos:
                    # Plan path to target (adjacent cell)
                    dest = (target_pos[0], target_pos[1] + 1)
                    path = self.bfs_path((self.x, self.y), dest)
                    if path:
                        self.path_queue = path
                        self.collection_phase = "MOVING_TO_DROP"
                        return self.path_queue.pop(0)
            else:
                # Find nearest uncollected colored box
                nearest_box = None
                min_dist = float('inf')
                
                for pos, btype in self.found_boxes.items():
                    if btype in ['r', 'g', 'b']:
                        dist = abs(self.x - pos[0]) + abs(self.y - pos[1])
                        if dist < min_dist:
                            min_dist = dist
                            nearest_box = pos
                
                if nearest_box:
                    # Plan path to box
                    # We want to go TO the box location to "pick it up"
                    # But wait, is it an obstacle? Yes.
                    # So BFS needs to allow entering the target node even if it is an obstacle.
                    path = self.bfs_path((self.x, self.y), nearest_box)
                    if path:
                        self.path_queue = path
                        self.collection_phase = "MOVING_TO_BOX"
                        self.target_box_pos = nearest_box
                        return self.path_queue.pop(0)
                else:
                    self.state = "FINISHED"

        return None

    def bfs_path(self, start, end):
        queue = [(start, [])]
        visited = set()
        visited.add(start)
        
        while queue:
            (curr, path) = queue.pop(0)
            
            if curr == end:
                return path
            
            for neighbor in self.get_neighbors(curr):
                # Valid neighbor?
                if neighbor not in visited:
                    # Check if obstacle (unless it's the destination box)
                    if self.grid_map.is_obstacle(neighbor[0], neighbor[1]) and neighbor != end:
                        continue
                        
                    visited.add(neighbor)
                    queue.append((neighbor, path + [neighbor]))
        return []

    def get_neighbors(self, pos):
        x, y = pos
        candidates = [(x, y-1), (x, y+1), (x-1, y), (x+1, y)]
        valid = []
        for cx, cy in candidates:
            if self.is_valid_pos(cx, cy):
                valid.append((cx, cy))
        return valid

    def calculate_dynamic_bypass(self, blocked_x, blocked_y):
        """ Attempts to find a path around an obstacle. """
        candidate_lanes = []
        
        # Option A: UP (y-1)
        if self.is_valid_pos(self.x, self.y - 1) and not self.grid_map.is_obstacle(self.x, self.y - 1):
            candidate_lanes.append(self.y - 1)
        # Option B: DOWN (y+1)
        if self.is_valid_pos(self.x, self.y + 1) and not self.grid_map.is_obstacle(self.x, self.y + 1):
            candidate_lanes.append(self.y + 1)
            
        if not candidate_lanes: return False 

        for bypass_y in candidate_lanes:
            temp_path = []
            lane_success = True 
            re_entry_found = False
            
            # Step 1: Move Sideways
            temp_path.append((self.x, bypass_y))
            curr_x = self.x
            
            # Step 2: Look ahead
            for i in range(1, GRID_SIZE + 1):
                next_check_x = self.x + (i * self.direction)
                
                # A. Check Edge of Map (SUCCESS CASE)
                if not self.is_valid_pos(next_check_x, bypass_y):
                    re_entry_found = True 
                    break
                    
                # B. Check for Obstacles in the Bypass Lane
                if self.grid_map.is_obstacle(next_check_x, bypass_y):
                    lane_success = False 
                    break 
                
                temp_path.append((next_check_x, bypass_y))
                curr_x = next_check_x
                
                # C. Check Re-entry to original row
                if self.is_valid_pos(curr_x, self.y) and not self.grid_map.is_obstacle(curr_x, self.y):
                    temp_path.append((curr_x, self.y)) 
                    re_entry_found = True
                    break
            
            if lane_success and re_entry_found:
                self.move_queue.extend(temp_path)
                return True
                
        return False

    def scan_and_report_local(self):
        self.scan_beams = [] 
        check_list = [(self.x, self.y - 1), (self.x, self.y + 1), (self.x + self.direction, self.y)]
        
        for nx, ny in check_list:
            if self.is_valid_pos(nx, ny):
                self.add_scan_beam(nx, ny)
                if self.grid_map.get_target(nx, ny):
                    rec = f"Target at ({nx},{ny})"
                    if rec not in self.grid_map.recorded_items: self.grid_map.recorded_items.append(rec)
                if self.grid_map.is_obstacle(nx, ny):
                    self.grid_map.known_obstacles.add((nx, ny))
                    obs_type = self.grid_map.obstacles[(nx, ny)]
                    self.found_boxes[(nx, ny)] = obs_type
                    rec = f"Box ({obs_type}) at ({nx},{ny})"
                    if rec not in self.grid_map.recorded_items: self.grid_map.recorded_items.append(rec)

    def add_scan_beam(self, tx, ty):
        start = (self.pixel_x, self.pixel_y)
        end = (self.grid_to_pixel(tx), self.grid_to_pixel(ty))
        self.scan_beams.append((start, end))

    def queue_row_change(self):
        self.current_row_idx += 1
        if self.current_row_idx >= len(self.scan_rows):
            self.state = "COLLECTING"
            return
        target_row = self.scan_rows[self.current_row_idx]
        temp_y = self.y 
        step = -1 if target_row < self.y else 1
        while temp_y != target_row:
            temp_y += step
            self.move_queue.append((self.x, temp_y))
        self.direction *= -1 

    def draw(self, surface):
        pygame.draw.circle(surface, COLOR_ROBOT, (int(self.pixel_x), int(self.pixel_y)), 15)
        end_pos = (self.pixel_x + (20 * self.direction), self.pixel_y)
        pygame.draw.line(surface, (0,0,0), (self.pixel_x, self.pixel_y), end_pos, 3)
        for start, end in self.scan_beams:
            pygame.draw.line(surface, COLOR_SCAN_LINE, start, end, 2)

# --- Main ---

def main():
    pygame.init()
    screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
    pygame.display.set_caption("Robot Simulation - 6 Boxes No Edge")
    clock = pygame.time.Clock()
    font = pygame.font.SysFont("Consolas", 14)
    btn_font = pygame.font.SysFont("Arial", 18, bold=True)

    grid_map = GridMap()
    robot = Robot(grid_map)
    btn_rect = pygame.Rect(430, 580, 140, 40)

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            
            if event.type == pygame.MOUSEBUTTONDOWN:
                if btn_rect.collidepoint(event.pos):
                    grid_map = GridMap()
                    robot = Robot(grid_map)

        robot.update()
        screen.fill(COLOR_BG)
        
        # Draw Grid
        for i in range(GRID_SIZE):
            p = MARGIN + i * CELL_SIZE
            pygame.draw.line(screen, COLOR_GRID, (p, MARGIN), (p, MARGIN + (GRID_SIZE - 1) * CELL_SIZE), 2)
            pygame.draw.line(screen, COLOR_GRID, (MARGIN, p), (MARGIN + (GRID_SIZE - 1) * CELL_SIZE, p), 2)

        # Draw Objects
        for (ox, oy), _ in grid_map.obstacles.items():
            cx, cy = MARGIN + ox * CELL_SIZE, MARGIN + oy * CELL_SIZE
            pygame.draw.rect(screen, COLOR_OBS, (cx - 12, cy - 12, 24, 24))

        for (kx, ky) in grid_map.known_obstacles:
            cx, cy = MARGIN + kx * CELL_SIZE, MARGIN + ky * CELL_SIZE
            # Draw based on type if known
            obs_type = robot.found_boxes.get((kx, ky), 'obstacle')
            color = (255, 255, 255) # Default white for obstacle
            if obs_type == 'r': color = (255, 0, 0)
            elif obs_type == 'g': color = (0, 255, 0)
            elif obs_type == 'b': color = (0, 0, 255)
            
            pygame.draw.rect(screen, color, (cx - 15, cy - 15, 30, 30), 2)

        for (tx, ty), color in grid_map.targets.items():
            cx, cy = MARGIN + tx * CELL_SIZE, MARGIN + ty * CELL_SIZE
            pygame.draw.rect(screen, color, (cx - 10, cy - 10, 20, 20))

        robot.draw(screen)
        
        # Draw Carried Box
        if robot.carrying:
            color = (255, 255, 255)
            if robot.carrying == 'r': color = (255, 0, 0)
            elif robot.carrying == 'g': color = (0, 255, 0)
            elif robot.carrying == 'b': color = (0, 0, 255)
            # Draw small box on top of robot
            pygame.draw.rect(screen, color, (robot.pixel_x - 8, robot.pixel_y - 8, 16, 16))

        # UI
        y_ui = SCREEN_HEIGHT - 100
        pygame.draw.rect(screen, (30,30,30), (0, y_ui, SCREEN_WIDTH, 100))
        txt_state = font.render(f"Pos: ({robot.x}, {robot.y}) | State: {robot.state}", True, (255, 255, 255))
        screen.blit(txt_state, (20, y_ui + 10))
        
        pygame.draw.rect(screen, COLOR_BUTTON, btn_rect, border_radius=5)
        btn_txt = btn_font.render("Random Map", True, (255, 255, 255))
        txt_rect = btn_txt.get_rect(center=btn_rect.center)
        screen.blit(btn_txt, txt_rect)

        pygame.display.flip()
        clock.tick(30) 

    pygame.quit()

if __name__ == "__main__":
    main()