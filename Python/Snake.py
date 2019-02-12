from threading import Thread, Timer, Lock
import threading
import numpy as np
from Display import Display
from copy import deepcopy
from random import randint


class Snake:

    class Enums:

        class Direction:
            U = 0  # Up
            D = 1  # Down
            L = 2  # Left
            R = 3  # Right
            NONE = 4

    class Part:

        def __init__(self, x, y):
            self.position = np.matrix(([x, y]), dtype=int)
            self.r = 10
            self.g = 100
            self.b = 10

        def get_pos(self):
            return self.position

        def __eq__(self, other):
            return np.array_equal(self.position, other.position)

    def __init__(self, w, h, callback):
        self.width = w
        self.height = h
        self.callback = callback
        #self.timer = Timer(interval=1, function=self.tick)
        self.timer = None
        self.default_tick_time = 0.2
        self.tick_time = self.default_tick_time
        self.mutex = Lock()
        self.dir = Snake.Enums.Direction.NONE
        self.body = [Snake.Part(self.width / 2, self.height / 2)]
        self.apples = []
        self.state = []
        self.on_apple = False
        self.game_started = False

        self.new_apple()
        self.draw()

        print("Size of apples: ", len(self.apples))

    def reset(self, path: str = "", arg1: int = -1):
        self.stop()
        self.dir = Snake.Enums.Direction.NONE
        self.body = [Snake.Part(self.width / 2, self.height / 2)]
        self.apples = []
        self.state = []
        self.on_apple = False
        self.game_started = False

        self.new_apple()
        self.draw()

        print("Game reset")

    def speed(self, path: str = "", arg1: float = 0.5):
        self.tick_time = self.default_tick_time + (0.5-arg1)*self.default_tick_time*1.0
        print("New Speed: ", self.tick_time)

    def turn(self, path: str, arg1: int):
        """
        Input is ["", "Snake", "Dir", "Up:Down:Left:Right"]
        """
        p = path.split('/')[3]

        if p == "Right" and self.dir is not Snake.Enums.Direction.L:
            self.dir = Snake.Enums.Direction.R
        elif p == "Left" and self.dir is not Snake.Enums.Direction.R:
            self.dir = Snake.Enums.Direction.L
        elif p == "Up" and self.dir is not Snake.Enums.Direction.D:
            self.dir = Snake.Enums.Direction.U
        elif p == "Down" and self.dir is not Snake.Enums.Direction.U:
            self.dir = Snake.Enums.Direction.D

        if self.game_started is False:
            self.game_started = True
            self.start()

    def colorize(self):
        l = len(self.body)
        i = 0
        for b in self.body:
            p = float(i)/float(l)
            b.r = int(float(255)*p)
            b.g = int(float(100)*p)
            b.b = int(float(150)*(1.0-p))
            i += 1

            for a in self.apples:
                if a == b:
                    dim = 1.0
                    a.r = int(dim*float(20)*(1-p) + float(b.r)*p)
                    a.g = int(dim*float(120) * (1 - p) + float(b.g) * p)
                    a.b = int(dim*float(15) * (1 - p) + float(b.b) * p)

    def advance_part(self, bodypart: Part, steps: int = 1) -> Part:
        """
        Translates a bodypart in the current direction
        Does not modify the argument
        If we reach the end of the screen we come out the other side

        :param bodypart: The bodypart use for translation
        :param steps:    Amount of steps to take
        :return:         A translated copy
        """
        if self.dir is Snake.Enums.Direction.NONE:
            return None

        b = deepcopy(bodypart)
        #print("Advancing...")
        if self.dir is Snake.Enums.Direction.R:
            b.position += np.matrix(([steps, 0]), dtype=int)
        elif self.dir is Snake.Enums.Direction.L:
            b.position += np.matrix(([-steps, 0]), dtype=int)
        elif self.dir is Snake.Enums.Direction.U:
            b.position += np.matrix(([0, -steps]), dtype=int)
        elif self.dir is Snake.Enums.Direction.D:
            b.position += np.matrix(([0, steps]), dtype=int)
        b.position[0, 0] = b.position[0, 0] % Display.WIDTH
        b.position[0, 1] = b.position[0, 1] % Display.HEIGHT
        return b

    def advance_snake(self):
        if self.body[0] is None:
            return

        new_head = self.advance_part(self.body[0])
        if new_head is None:
            return

        self.body.insert(0, new_head)

        # Let the snake grow until its 5 pixels long
        if len(self.body) > 5 and not self.on_apple:
            self.body.pop()  # Remove the tail

        self.on_apple = False

        new_apple = False
        tail = self.body[-1]
        #print(new_head.get_pos())
        for apple in self.apples[:]:
            #print(apple.get_pos())
            if new_head == apple:
                self.on_apple = True
                new_apple = True
            if tail == apple:
                self.apples.remove(apple)
                print("Ate apple")

        if self.self_collision():
            print("Game Over!")
            self.stop()
            self.dir = Snake.Enums.Direction.NONE
            #self.reset()
            return

        if new_apple:
            self.new_apple()

    def new_apple(self):
        """
        Generate a new apple that is not on the snake body
        """
        ok_position = False
        while ok_position is False:
            a = Snake.Part(randint(0, self.width - 1), randint(0, self.height - 1))
            if not self.on_snake_body(a):
                ok_position = True

        self.apples.append(a)

    def on_snake_body(self, p: Part) -> bool:
        for b in self.body:
            if p == b:
                return True
        return False

    def self_collision(self) -> bool:
        head = self.body[0]
        for b in self.body[1:]:
            if b == head:
                return True
        return False

    def draw(self):

        self.colorize()

        #print("Size of Body: ", len(self.body))
        self.state = []
        for b in self.body:
            pos = b.get_pos()
            self.state.append(
                Display.Pixel(x=pos[0, 0],
                              y=pos[0, 1],
                              r=b.r,
                              g=b.g,
                              b=b.b))

        #print("Size of Apples: ", len(self.apples))
        for a in self.apples:
            pos = a.get_pos()
            self.state.append(
                Display.Pixel(x=pos[0, 0],
                              y=pos[0, 1],
                              r=a.r,
                              g=a.g,
                              b=a.b))

        self.callback(self.state)

    def start(self):
        #print("Start")
        self.tick()
        #print("Start end")

    def tick(self):
        #print("Tick")
        self.mutex.acquire()
        try:
            #thread_id = threading.get_ident()
            #print('\nProcessing data:', self.dir, "ThreadId:", thread_id)
            self.advance_snake()
            self.draw()
            self.callback(self.state)

        finally:
            self.mutex.release()

        t = Timer(self.tick_time, self.tick)
        self.timer = t
        t.start()

    def stop(self):
        if self.timer is not None:
            self.timer.cancel()

