import pygame
import random
import math

from Datatypes import Node, LinkNode

# ---------- Constants ----------
GRID_SIZE = 8
PADDING_RATIO = 0.1
WINDOW_HEIGHT = 600
WINDOW_WIDTH = 600
N_OBSTACLES = 4  # number of obstacles

# ---------- Data classes ----------
class GridNode:
    """
    state ∈ {
      'empty','obstacle','start','end',
      'red_dropoff','blue_dropoff','green_dropoff',
      'red_pickup','blue_pickup','green_pickup'
    }
    If a box is dropped on a drop-off, we show it via box_color overlay ('red'/'blue'/'green').
    """
    def __init__(self, x, y, state):
        self.x, self.y = x, y
        self.state = state
        self.walkable = state in {
            'empty', 'start', 'end',
            'red_dropoff', 'blue_dropoff', 'green_dropoff',
            'red_pickup', 'blue_pickup', 'green_pickup'
        }
        self.box_color = None  # overlayed box on drop-off tiles (None/'red'/'blue'/'green')

class Grid:
    def __init__(self, grid_size=GRID_SIZE, objects=None):
        self.size = grid_size
        # build empty grid
        self.nodes = [[GridNode(x, y, 'empty') for x in range(self.size + 1)]
                      for y in range(self.size + 1)]

        # fixed nodes
        self.start = (0, 8)
        self.end = (8, 0)
        self.dropoffs = {
            (2, 0): 'red_dropoff',
            (4, 0): 'blue_dropoff',
            (6, 0): 'green_dropoff',
        }

        # --- reserve cells that must not be overwritten ---
        reserved = set()
        reserved.add(self.start)
        reserved.add(self.end)
        reserved.update(self.dropoffs.keys())

        if objects is not None:
            # accept both "obstacles" and the common typo "obstables"
            obj_obstacles = objects.get("obstacles", objects.get("obstables", []))

            # place obstacles
            for (ox, oy) in obj_obstacles:
                if 0 <= ox <= self.size and 0 <= oy <= self.size and (ox, oy) not in reserved:
                    self.nodes[oy][ox] = GridNode(ox, oy, 'obstacle')

            # place color pickups (keys: "red", "blue", "green"), e.g. "green": (1,2)
            for color in ("red", "blue", "green"):
                if color in objects:
                    px, py = objects[color]
                    if 0 <= px <= self.size and 0 <= py <= self.size and (px, py) not in reserved:
                        self.nodes[py][px] = GridNode(px, py, f"{color}_pickup")
        else:
            # ---- original random placement fallback ----
            # obstacles
            obstacles = []
            while len(obstacles) < N_OBSTACLES:
                x = random.randint(1, self.size - 1)
                y = random.randint(1, self.size - 1)
                if (x, y) not in obstacles and (x, y) not in reserved:
                    obstacles.append((x, y))
                    self.nodes[y][x] = GridNode(x, y, 'obstacle')

            # two random pickup colors
            colors_to_place = random.sample(['red', 'blue', 'green'], 2)
            pickup_positions = []
            while len(pickup_positions) < 2:
                x = random.randint(0, self.size)
                y = random.randint(1, self.size - 1)  # avoid y=0 (drop-off row)
                if (x, y) not in reserved and (x, y) not in pickup_positions:
                    pickup_positions.append((x, y))

            for (x, y), color in zip(pickup_positions, colors_to_place):
                self.nodes[y][x] = GridNode(x, y, f'{color}_pickup')

        # ---- place fixed nodes last (these override anything underneath) ----
        sx, sy = self.start
        ex, ey = self.end
        self.nodes[sy][sx] = GridNode(sx, sy, 'start')
        self.nodes[ey][ex] = GridNode(ex, ey, 'end')
        for (x, y), s in self.dropoffs.items():
            self.nodes[y][x] = GridNode(x, y, s)


    # ---- helpers / physics ----
    def in_bounds(self, x, y):
        return 0 <= x <= self.size and 0 <= y <= self.size

    def node(self, x, y) -> GridNode:
        return self.nodes[y][x]

    def is_dropoff(self, x, y):
        return self.node(x, y).state in {'red_dropoff', 'blue_dropoff', 'green_dropoff'}

    def has_box(self, x, y):
        n = self.node(x, y)
        return (n.state in {'red_pickup', 'blue_pickup', 'green_pickup'}) or (n.box_color in {'red', 'blue', 'green'})

    def pickup_color_on_tile(self, x, y):
        n = self.node(x, y)
        if n.state == 'red_pickup': return 'red'
        if n.state == 'blue_pickup': return 'blue'
        if n.state == 'green_pickup': return 'green'
        # overlay on dropoff
        if n.box_color in {'red', 'blue', 'green'}:
            return n.box_color
        return None

    def clear_pickup_from_tile(self, x, y):
        """Remove a pickup or overlayed box from this tile, turning pickups into empty; dropoffs keep their state."""
        n = self.node(x, y)
        if n.state in {'red_pickup', 'blue_pickup', 'green_pickup'}:
            self.nodes[y][x] = GridNode(x, y, 'empty')
        elif n.state in {'red_dropoff', 'blue_dropoff', 'green_dropoff'}:
            n.box_color = None  # just clear overlay

    def place_pickup_on_empty(self, x, y, color):
        """Place a pickup tile on an empty cell."""
        assert color in {'red', 'blue', 'green'}
        self.nodes[y][x] = GridNode(x, y, f'{color}_pickup')

    def drop_on_dropoff(self, x, y, color):
        """Overlay a box on a drop-off tile (state unchanged)."""
        assert self.is_dropoff(x, y)
        self.node(x, y).box_color = color

    def can_enter(self, x, y, dx, dy):
        """
        Movement rule:
        - must be in bounds and walkable
        - if the destination tile "has a box" (pickup tile OR dropoff with overlay),
          you may NOT enter horizontally (long side). Vertical entry allowed.
        """
        if not self.in_bounds(x, y):
            return False
        n = self.node(x, y)
        if not n.walkable:
            return False
        entering_long_side = (dx != 0 and dy == 0)  # left/right
        if self.has_box(x, y) and entering_long_side:
            return False
        return True

    # ---- drawing ----
    def draw(self, surface, width, height):
        min_dim = min(width, height)
        padding = int(PADDING_RATIO * min_dim)
        grid_area = min_dim - 2 * padding
        cell_size = grid_area // self.size

        WHITE = (220, 220, 220)

        # grid lines
        for x in range(self.size + 1):
            pygame.draw.line(
                surface, WHITE,
                (padding + x * cell_size, padding),
                (padding + x * cell_size, padding + self.size * cell_size), 5
            )
        for y in range(self.size + 1):
            pygame.draw.line(
                surface, WHITE,
                (padding, padding + y * cell_size),
                (padding + self.size * cell_size, padding + y * cell_size), 5
            )

        # cells
        for row in self.nodes:
            for n in row:
                self._draw_node(surface, width, height, n, cell_size, padding)

    def _draw_node(self, surface, width, height, n: GridNode, cell_size, padding):
        colors = {
            'obstacle': (220, 220, 220),
            'start': (100, 100, 100),
            'end': (100, 100, 100),
            'red_dropoff': (255, 100, 100),
            'blue_dropoff': (100, 100, 255),
            'green_dropoff': (100, 255, 100),
            'red_pickup': (255, 0, 0),
            'blue_pickup': (0, 0, 255),
            'green_pickup': (0, 255, 0),
        }
        color = colors.get(n.state)
        if not color:
            return

        dropoff_states = {'red_dropoff', 'blue_dropoff', 'green_dropoff'}
        pickup_states = {'red_pickup', 'blue_pickup', 'green_pickup'}

        if n.state in dropoff_states:
            rect = pygame.Rect(
                padding + n.x * cell_size - cell_size // 2 + 5,
                padding + n.y * cell_size - cell_size // 2 + 5,
                cell_size - 10,
                cell_size - 10
            )
        elif n.state in pickup_states:
            # tall, narrow pickup box
            rect = pygame.Rect(
                padding + n.x * cell_size - cell_size // 10,
                padding + n.y * cell_size - cell_size // 5,
                cell_size // 5,
                cell_size // 2
            )
        else:
            rect = pygame.Rect(
                padding + n.x * cell_size - cell_size // 2 + 20,
                padding + n.y * cell_size - cell_size // 2 + 20,
                cell_size - 40,
                cell_size - 40
            )

        pygame.draw.rect(surface, color, rect)

        # overlayed box (if any) on dropoff
        if n.state in {'red_dropoff', 'blue_dropoff', 'green_dropoff'} and n.box_color in {'red', 'blue', 'green'}:
            oc = (255, 0, 0) if n.box_color == 'red' else (0, 0, 255) if n.box_color == 'blue' else (0, 255, 0)
            overlay_rect = pygame.Rect(
                padding + n.x * cell_size - cell_size // 10,
                padding + n.y * cell_size - cell_size // 5,
                cell_size // 5,
                cell_size // 2
            )
            pygame.draw.rect(surface, oc, overlay_rect)

class Robot:
    """
    Player-controlled robot that encapsulates all game physics:
    - Movement with cooldown (left/right/up/down)
      • First rotates to face the requested direction (no move this tick),
        then on the next call it moves if cooldown allows.
    - In-place rotation via rotate_inplace('left'/'right'/deg)
    - Long-side blocking onto box tiles (delegated to Grid.can_enter)
    - Short-side-only pickup (must have approached vertically)
    - Drop on any drop-off (overlay), or on empty (spawns pickup)
    - Sensing:
        • detect_box_left/front/right  (colored boxes only)
        • detect_block(where)          (obstacle OR box) — single-direction, like color peek
        • peek_box_color(where)        ('red'|'blue'|'green'|None)
    """
    def __init__(self, grid, x, y):
        self.grid = grid
        self.x, self.y = x, y
        self.carrying = None          # 'red' | 'blue' | 'green' | None
        self.orientation = 0          # 0:right, 90:up, 180:left, 270:down
        self.last_step = None         # (dx, dy) of last successful move

        # movement throttling
        self._last_move_time = 0
        self.MOVE_COOLDOWN_MS = 150

    # ---------- In-place rotation ----------
    def rotate_inplace(self, turn='left'):
        """
        Rotate without moving.
        turn:
          - 'left'  -> -90°
          - 'right' -> +90°
          - int/float -> absolute orientation in degrees (e.g., 0/90/180/270)
        """
        if isinstance(turn, (int, float)):
            self.orientation = int(turn) % 360
            return
        t = str(turn).lower()
        if t == 'left':
            delta = -90
        elif t == 'right':
            delta = +90
        else:
            raise ValueError("rotate_inplace(turn) expects 'left', 'right', or a degree number")
        self.orientation = (self.orientation + delta) % 360

    # ---------- Movement API ----------
    def left(self, now_ms=None):
        self._move(-1, 0, 180, now_ms)

    def right(self, now_ms=None):
        self._move(1, 0, 0, now_ms)

    def up(self, now_ms=None):
        self._move(0, -1, 90, now_ms)

    def down(self, now_ms=None):
        self._move(0, 1, 270, now_ms)

    # ---------- Pick / Drop ----------
    def pickup(self):
        """
        If not carrying: pick up a box on the current tile (pickup tile or drop-off overlay),
        BUT ONLY if the robot approached this tile vertically (short side).
        If carrying: drop onto any drop-off (overlay) or onto empty (spawns pickup).
        """
        cx, cy = self.x, self.y
        node = self.grid.node(cx, cy)

        if self.carrying is None:
            # Require short-side (vertical) approach: dy != 0 on last successful step
            approached_short = (self.last_step is not None and self.last_step[1] != 0)
            if not approached_short:
                return

            color_here = self.grid.pickup_color_on_tile(cx, cy)
            if color_here in {'red', 'blue', 'green'}:
                self.carrying = color_here
                self.grid.clear_pickup_from_tile(cx, cy)  # remove pickup or overlay

        else:
            # Drop on ANY drop-off → overlay
            if self.grid.is_dropoff(cx, cy):
                self.grid.drop_on_dropoff(cx, cy, self.carrying)
                self.carrying = None
                return

            # Drop on empty → convert to pickup tile
            if node.state == 'empty':
                self.grid.place_pickup_on_empty(cx, cy, self.carrying)
                self.carrying = None

    # ---------- Sensing (separate helpers) ----------
    # Boxes ONLY (colored pickups or drop-off overlays)
    def detect_box_left(self) -> bool:
        nx, ny = self._neighbor_coords_lfr()["left"]
        return self.grid.in_bounds(nx, ny) and self.grid.has_box(nx, ny)

    def detect_box_front(self) -> bool:
        nx, ny = self._neighbor_coords_lfr()["front"]
        return self.grid.in_bounds(nx, ny) and self.grid.has_box(nx, ny)

    def detect_box_right(self) -> bool:
        nx, ny = self._neighbor_coords_lfr()["right"]
        return self.grid.in_bounds(nx, ny) and self.grid.has_box(nx, ny)

    # Blocking = obstacle OR box (single-direction, like color sensing)
    def detect_block(self, where="front") -> bool:
        where = where.lower()
        if where not in {"left", "front", "right"}:
            raise ValueError("where must be one of {'left','front','right'}")
        nx, ny = self._neighbor_coords_lfr()[where]
        if not self.grid.in_bounds(nx, ny):
            return False
        n = self.grid.node(nx, ny)
        return (n.state == 'obstacle') or self.grid.has_box(nx, ny)

    def peek_box_color(self, where="front"):
        """
        Return 'red' | 'blue' | 'green' | None for the adjacent tile in
        where ∈ {'left','front','right'}.
        Works for both pickup tiles and drop-off overlays (never 'obstacle').
        """
        where = where.lower()
        if where not in {"left", "front", "right"}:
            raise ValueError("where must be one of {'left','front','right'}")
        nx, ny = self._neighbor_coords_lfr()[where]
        if not self.grid.in_bounds(nx, ny):
            return None
        return self.grid.pickup_color_on_tile(nx, ny)

    # ---------- Drawing ----------
    def draw(self, surface, width, height):
        min_dim = min(width, height)
        padding = int(PADDING_RATIO * min_dim)
        grid_area = min_dim - 2 * padding
        cell_size = grid_area // self.grid.size

        center_x = padding + self.x * cell_size
        center_y = padding + self.y * cell_size
        radius = cell_size // 4

        # body
        pygame.draw.ellipse(
            surface, (255, 255, 0),
            (center_x - radius, center_y - radius, 2 * radius, 2 * radius)
        )

        # direction indicator
        dir_len = radius
        ang = math.radians(self.orientation)
        end_x = center_x + dir_len * math.cos(ang)
        end_y = center_y - dir_len * math.sin(ang)
        pygame.draw.line(surface, (0, 0, 0), (center_x, center_y), (end_x, end_y), 3)

        # carried item bubble
        if self.carrying:
            col = (255, 0, 0) if self.carrying == 'red' else (0, 0, 255) if self.carrying == 'blue' else (0, 255, 0)
            pygame.draw.circle(surface, col, (center_x, center_y - radius - 10), radius // 2)

    # ---------- Internals ----------
    def _move(self, dx, dy, face_deg, now_ms=None):
        """
        Rotate-first behavior:
          - If not facing the requested direction, just rotate this tick (no move).
          - If already facing, move if cooldown allows and Grid.can_enter(..) permits.
        """
        # Always rotate immediately (no cooldown on rotation)
        desired = face_deg % 360
        if (self.orientation % 360) != desired:
            self.orientation = desired
            return  # rotate-only this tick

        # Movement cooldown check
        if now_ms is None:
            now_ms = pygame.time.get_ticks()
        if now_ms - self._last_move_time < self.MOVE_COOLDOWN_MS:
            return

        # Attempt to move
        nx, ny = self.x + dx, self.y + dy
        if self.grid.can_enter(nx, ny, dx, dy):
            self.x, self.y = nx, ny
            self.last_step = (dx, dy)

        self._last_move_time = now_ms

    def _neighbor_coords_lfr(self):
        """Return neighbor coords for left/front/right based on current orientation."""
        # Quantize orientation to nearest axis (0/90/180/270)
        theta = int(round(self.orientation / 90.0)) * 90 % 360

        if theta == 0:        # facing right
            fdx, fdy = (1, 0)
        elif theta == 90:     # facing up
            fdx, fdy = (0, -1)
        elif theta == 180:    # facing left
            fdx, fdy = (-1, 0)
        else:                 # 270, facing down
            fdx, fdy = (0, 1)

        # left/right are perpendicular to front
        ldx, ldy = -fdy, fdx
        rdx, rdy = fdy, -fdx

        return {
            "left":  (self.x + ldx,  self.y + ldy),
            "front": (self.x + fdx,  self.y + fdy),
            "right": (self.x + rdx,  self.y + rdy),
        }


def draw_diagnostics(surface, robot, width, height, font):
    # Box sensors
    box_l  = robot.detect_box_left()
    box_f  = robot.detect_box_front()
    box_r  = robot.detect_box_right()

    # Single-direction blocking (front), like color sensing
    blk_f  = robot.detect_block("front")
    front_col = robot.peek_box_color("front")  # 'red'|'blue'|'green'|None

    def on_off_color(val): return (0, 200, 0) if val else (160, 160, 160)
    color_map = {
        None:  (200, 200, 200),
        "red": (255, 0, 0),
        "blue": (0, 0, 255),
        "green": (0, 200, 0),
    }

    x0, y0 = 12, 12
    line_h = font.get_height() + 4

    lines_count = 5  # Box L/F/R + BlockFront + FrontColor
    bg_w = 240
    bg_h = line_h * lines_count + 12
    bg = pygame.Surface((bg_w, bg_h), pygame.SRCALPHA)
    bg.fill((0, 0, 0, 120))
    surface.blit(bg, (x0 - 8, y0 - 8))

    out_lines = [
        (f"Box L: {box_l}",       on_off_color(box_l)),
        (f"Box F: {box_f}",       on_off_color(box_f)),
        (f"Box R: {box_r}",       on_off_color(box_r)),
        (f"Block F: {blk_f}",     on_off_color(blk_f)),
    ]
    for i, (text, col) in enumerate(out_lines):
        surface.blit(font.render(text, True, col), (x0, y0 + i * line_h))

    fc_text = "None" if front_col is None else front_col.capitalize()
    fc_col  = color_map[front_col]
    surface.blit(font.render(f"FrontColor: {fc_text}", True, fc_col), (x0, y0 + 4 * line_h))

# ---------- Pygame setup & main loop ----------
def arrows():
    pygame.init()
    screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT), pygame.RESIZABLE)
    pygame.display.set_caption("Maze Grid 8x8 – Class-based")
    font = pygame.font.SysFont(None, 20)  # or any size you like


    grid = Grid(GRID_SIZE)
    sx, sy = grid.start
    robot = Robot(grid, sx, sy)

    BLACK = (0, 0, 0)

    # simple space rising-edge
    space_prev = False

    running = True
    width, height = pygame.display.get_surface().get_size()
    clock = pygame.time.Clock()

    while running:
        now = pygame.time.get_ticks()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.VIDEORESIZE:
                width, height = event.w, event.h

        keys = pygame.key.get_pressed()
        if keys[pygame.K_LEFT]:
            robot.left(now)
        elif keys[pygame.K_RIGHT]:
            robot.right(now)
        elif keys[pygame.K_UP]:
            robot.up(now)
        elif keys[pygame.K_DOWN]:
            robot.down(now)

        space_now = keys[pygame.K_SPACE]
        if space_now and not space_prev:
            robot.pickup()
        space_prev = space_now

        # draw
        screen.fill(BLACK)
        grid.draw(screen, width, height)
        robot.draw(screen, width, height)
        draw_diagnostics(screen, robot, width, height, font)  # <-- NEW
        pygame.display.flip()


        clock.tick(60)  # cap FPS
        

    pygame.quit()


def pathFinder(start:Node, end:Node, concat:LinkNode=LinkNode("end")):
    dx = end.x - start.x
    dy = end.y - start.y

    if dx == 0 and dy==0:
        return concat
    
    if dx != 0:
        if dx > 0:
            newNode = Node(start.x+1, start.y)
            return LinkNode("right", pathFinder(newNode, end, concat))
        if dx < 0:
            newNode = Node(start.x-1, start.y)
            return LinkNode("left", pathFinder(newNode, end, concat))
    else:
        if dy > 0:
            newNode = Node(start.x, start.y+1)
            return LinkNode("up", pathFinder(newNode, end, concat))
        if dy < 0:
            newNode = Node(start.x, start.y-1)
            return LinkNode("down", pathFinder(newNode, end, concat))

def getPosAfter(start:Node, path:LinkNode, steps: int):
    for i in range(steps):
        match path.value:
            case "right":
                start = start.right()
            case "left":
                start = start.left()
            case "up":
                start = start.up()
            case "down":
                start = start.down()
        path = path.next
    return start



def redirect(start: Node, currentPath: LinkNode, block: list[Node] = []):
    """
    Returns a new path if redirection occurs; otherwise returns currentPath unchanged.
    """
    match currentPath.value:
        case "right":
            if start.right() in block:
                return LinkNode("up", pathFinder(
                    start.up(),
                    getPosAfter(start, currentPath, 2),
                    currentPath.next.next if currentPath.next and currentPath.next.next else LinkNode("end")
                ))

        case "left":
            if start.left() in block:
                return LinkNode("up", pathFinder(
                    start.up(),
                    getPosAfter(start, currentPath, 2),
                    currentPath.next.next if currentPath.next and currentPath.next.next else LinkNode("end")
                ))

        case "up":
            if start.up() in block:
                return LinkNode("right", pathFinder(
                    start.right(),
                    getPosAfter(start, currentPath, 2),
                    currentPath.next.next if currentPath.next and currentPath.next.next else LinkNode("end")
                ))

        case "down":
            if start.down() in block:
                return LinkNode("right", pathFinder(
                    start.right(),
                    getPosAfter(start, currentPath, 2),
                    currentPath.next.next if currentPath.next and currentPath.next.next else LinkNode("end")
                ))

    return currentPath  # if no redirection

            
def move(robot:Robot, path:LinkNode):
    match path.value:
        case "right":
            robot.right()
        case "left":
            robot.left()
        case "up":
            robot.up()
        case "down":
            robot.down()

blocks = []
def moveOnPath(robot: Robot, path: LinkNode, oldPos: Node): 
    if path is None or path.value == "end":
        return path

    newPos = Node(robot.x, robot.y)

    if path.next is not None:
        match path.value:
            case "right":
                if robot.detect_block():
                    blocks.append(newPos.right())
                    print("blocks : ", blocks)
                    path = redirect(newPos, path, blocks)
                    path.print()
                    move(robot, path)
                else:
                    robot.right()
            case "left":
                if robot.detect_block():
                    blocks.append(newPos.left())
                    print("blocks : ", blocks)
                    path = redirect(newPos, path, blocks)
                    path.print()
                    move(robot, path)
                else:
                    robot.left()
            case "up":
                if robot.detect_block():
                    blocks.append(newPos.up())
                    print("blocks : ", blocks)
                    path = redirect(newPos, path, blocks)
                    path.print()
                    move(robot, path)
                else:
                    robot.up()
            case "down":
                if robot.detect_block():
                    blocks.append(newPos.down())
                    print("blocks : ", blocks)
                    path = redirect(newPos, path, blocks)
                    path.print()
                    move(robot, path)
                else:
                    robot.down()

        # update position only after movement
        newPos = Node(robot.x, robot.y)
        if newPos != oldPos:
            print("Moved:", path.value)
            path = path.next  # advance path

    return path

def main():
    pygame.init()
    screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT), pygame.RESIZABLE)
    pygame.display.set_caption("Maze Grid 8x8 – Class-based")
    font = pygame.font.SysFont(None, 20)  # or any size you like

    objects = {
        "obstables": [(2,7), (3,6)],
        "green": (1,1) 
    }

    grid = Grid(GRID_SIZE, objects)
    sx, sy = grid.start
    robot = Robot(grid, sx, sy)

    BLACK = (0, 0, 0)

    
    # simple space rising-edge
    space_prev = False

    running = True
    width, height = pygame.display.get_surface().get_size()
    clock = pygame.time.Clock()

    i,j = 0,0

    path = LinkNode("up", pathFinder(Node(0,1), Node(7,1)))

    while running:
        now = pygame.time.get_ticks()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.VIDEORESIZE:
                width, height = event.w, event.h

        oldPos = Node(robot.x, robot.y)
        newPath  = moveOnPath(robot, path, oldPos)
        if newPath is not None:
            path = newPath

        # draw
        screen.fill(BLACK)
        grid.draw(screen, width, height)
        robot.draw(screen, width, height)
        draw_diagnostics(screen, robot, width, height, font)  # <-- NEW
        pygame.display.flip()


        clock.tick(60)  # cap FPS
        

    pygame.quit()


if __name__ == "__main__":
    # arrows()
    main()
