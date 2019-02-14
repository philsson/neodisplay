import mt
import socket
from Display import Display
from time import sleep

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP


class DisplayDataSender:
    def __init__(self, dest, port):
        self.loopObj = None
        self.dest = dest
        self.port = port

        # Walking pixel
        self.walking_pixel = None
        self.prev_walking_pixel = None

    def slider(self, path, tags, args, source):
        # Convert to 0,0 at Upper left corner
        x, y = round(args[1]*(Display.WIDTH-1)), round(7-args[0]*(Display.HEIGHT-1))
        index = Display.index_from_xy(x, y)

        print("updating x: ", x, " y: ", y, " index: ", index)

        pixels = [mt.Pixel(Index=index, Red=100, Green=30, Blue=100)]

        payload = mt.Pixels(
            TypeOfUpdate=mt.DisplayEnums.DisplayUpdate.FULL,
            NumberOfPixelUpdates=len(pixels),
            PixelData=pixels
        )

        header = mt.Header(
            MessageDataType=mt.DisplayEnums.PacketType.DISPLAY_INPUT,
            MessageDataSize=len(payload)
        )

        self.send_packet(header, payload)

    def send_packet(self, header, payload):
        sock.sendto(bytes(header / payload), (self.dest, self.port))

    def clear(self, layer=1):

        payload = mt.Command(
            Command=mt.DisplayEnums.Command.CLEAR,
            Value=layer
        )

        header = mt.Header(
            MessageDataType=mt.DisplayEnums.PacketType.COMMAND,
            MessageDataSize=len(payload)
        )

        self.send_packet(header, payload)

    def walk_pixel(self, layer=mt.DisplayEnums.Layer.FOREGROUND):

        pixels = []

        if self.walking_pixel is not None:
            self.prev_walking_pixel = mt.Pixel(Index=self.walking_pixel.Index, Red=0, Green=0, Blue=0)
            pixels.append(self.prev_walking_pixel)
            self.walking_pixel.Index = (self.walking_pixel.Index + 1) % 256
            #print(self.walking_pixel.Index)
        else:
            self.walking_pixel = mt.Pixel(Index=0, Red=100, Green=100, Blue=100)

        pixels.append(self.walking_pixel)

        payload = mt.Pixels(
            TypeOfUpdate=mt.DisplayEnums.DisplayUpdate.PARTIAL,
            Layer=layer,
            NumberOfPixelUpdates=len(pixels),
            PixelData=pixels
        )

        header = mt.Header(
            MessageDataType=mt.DisplayEnums.PacketType.DISPLAY_INPUT,
            MessageDataSize=len(payload)
        )

        #payload.show()
        self.send_packet(header, payload)

    def send_command(self, command: mt.DisplayEnums.Command, value=0, value2=0):

        payload = mt.Command(
            Command=command,
            Value=value,
            Value2=value2
        )

        header = mt.Header(
            MessageDataType=mt.DisplayEnums.PacketType.COMMAND,
            MessageDataSize=len(payload)
        )

        self.send_packet(header, payload)

    def set_mode(self, value):

        payload = mt.Mode(
            Mode=value,
        )

        header = mt.Header(
            MessageDataType=mt.DisplayEnums.PacketType.MODE,
            MessageDataSize=len(payload)
        )

        self.send_packet(header, payload)

    def set_effect(self, value, permanent=True):
        payload = mt.Effect(
            Effect=value,
            Permanent=permanent
        )

        header = mt.Header(
            MessageDataType=mt.DisplayEnums.PacketType.EFFECT,
            MessageDataSize=len(payload)
        )

        self.send_packet(header, payload)

    def update(self, pixels: [Display.Pixel]):

        pixels_out = []

        for pixel in pixels:
            pixel_out = mt.Pixel(Index=Display.index_from_xy(pixel.x, pixel.y),
                                 Red=pixel.r,
                                 Green=pixel.g,
                                 Blue=pixel.b)
            pixels_out.append(pixel_out)

        payload = mt.Pixels(
            TypeOfUpdate=mt.DisplayEnums.DisplayUpdate.FULL,
            Layer=mt.DisplayEnums.Layer.FOREGROUND,
            NumberOfPixelUpdates=len(pixels_out),
            PixelData=pixels_out
        )

        header = mt.Header(
            MessageDataType=mt.DisplayEnums.PacketType.DISPLAY_INPUT,  # 1 is command
            MessageDataSize=len(payload)
        )

        self.send_packet(header, payload)

    def set_brightness(self, value):

        if value > 255:
            value = 255
        elif value < 0:
            value = 0

        self.send_command(mt.DisplayEnums.Command.BRIGHTNESS, value)

    def set_layer_brightness(self, layer: mt.DisplayEnums.Layer, value):
        if value > 255:
            value = 255
        elif value < 0:
            value = 0

        self.send_command(mt.DisplayEnums.Command.LAYER_BRIGHTNESS, layer, value)

    def update2(self):

        payloadMode = mt.Mode(
            Mode=2
        )

        headerMode = mt.Header(
            MessageDataType=mt.DisplayEnums.PacketType.MODE,
            MessageDataSize=len(payloadMode)
        )

        payloadCommand = mt.Command(
            Command=mt.DisplayEnums.Command.TEST,
            Value=20
        )

        headerCommand = mt.Header(
            MessageDataType=mt.DisplayEnums.PacketType.COMMAND,
            MessageDataSize=len(payloadCommand)
        )



        pixels = []
        pixels.append(mt.Pixel(Index=0))
        pixels.append(mt.Pixel(Index=1))
        pixels.append(mt.Pixel(Index=2))
        pixels.append(mt.Pixel(Index=3))

        payloadPixels = mt.Pixels(
            TypeOfUpdate=3,
            NumberOfPixelUpdates=len(pixels),
            PixelData=pixels
        )

        headerPixels = mt.Header(
            MessageDataType=2,  # 1 is command
            MessageDataSize=len(payloadPixels)
        )

        #sock.sendto(str(headerMode / payloadMode), (self.dest, self.port))


        #return
        #sock.sendto(str(headerCommand / payloadCommand), (self.dest, self.port))

        #return

        #sock.sendto(str(headerPixels / payloadPixels), (self.dest, self.port))

        #headerPixels.show()
        #payloadPixels.show()

        #sleep(10)

        framerate = 50
        div = 255/float(framerate)
        s = 1.0/float(framerate)
        print("goal:", framerate, " div:", div, " s:", s)
        while True:
            for i in range(0, framerate):
                for k in range(0, 4):
                    payloadPixels.PixelData[0].Red = int(i*div)
                    payloadPixels.PixelData[2].Green = int((framerate-i)*div)
                    sock.sendto(str(headerPixels / payloadPixels), (self.dest, self.port))
                sleep(s)
            for i in range(0, framerate):
                for k in range(0, 4):
                    payloadPixels.PixelData[0].Red = int((framerate-i)*div)
                    payloadPixels.PixelData[2].Green = int(i*div)
                sock.sendto(str(headerPixels / payloadPixels), (self.dest, self.port))
                sleep(s)