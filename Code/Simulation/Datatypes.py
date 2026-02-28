class Node:
    def __init__(self, x=0, y=0):
        self.x = x
        self.y = y

    def __eq__(self, n1):
        return self.x == n1.x and self.y == n1.y

    def __ne__(self, n1):
        return self.x != n1.x or self.y != n1.y
    
    def isValid(self):
        return (self.x<9 and self.x>-1) or (self.y<9 and self.y>-1)
    
    def nextPos(self, x, y):
        pos = Node(self.x+x, self.y+y)
        if pos.isValid():
            return pos
        raise IndexError(f"Position (${self.x}, ${self.y}) is not valid")

    def right(self):
        return self.nextPos(1, 0)
    def left(self):
        return self.nextPos(-1, 0)
    def up(self):
        return self.nextPos(0, 1)
    def down(self):
        return self.nextPos(0, -1)

class LinkNode():
    def __init__(self, value, next=None):
        self.value = value
        self.next = next

    def print(self):
        link = self

        while True:
            print(link.value)
            if link.next == None:
                return
            link = link.next


