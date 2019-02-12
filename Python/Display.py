
class Display:

    NUM_PIXELS = 256

    UPPER_LEFT = 0
    LOWER_LEFT = 7
    UPPER_RIGHT = 255
    LOWER_RIGHT = 248

    WIDTH = 32
    HEIGHT = 8

    class Pixel:
        def __init__(self, x: int, y: int, r: int, g: int, b: int):
            self.x = int(x)
            self.y = int(y)
            self.r = int(r)
            self.g = int(g)
            self.b = int(b)

    @staticmethod
    def index_from_xy(x: int, y: int):
        offset_y = 0
        if Display.UPPER_LEFT == 0:
            offset_y = (y if x % 2 == 0 else Display.LOWER_LEFT -y)
        output = x * (Display.LOWER_LEFT + 1) + offset_y
        if output < 0:
            return 0
        elif output >= 256:
            return 255
        return int(output)
