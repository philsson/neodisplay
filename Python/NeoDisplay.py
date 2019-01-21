from time import sleep
import socket

import mt

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

    def send_packet(self, header, payload):

        sock.sendto(str(header / payload), (self.dest, self.port))

    def clear(self):

        payload = mt.Command(
            Command=mt.DisplayEnums.Command.CLEAR,
            Value=0
        )

        header = mt.Header(
            MessageDataType=mt.DisplayEnums.PacketType.COMMAND,
            MessageDataSize=len(payload)
        )

        self.send_packet(header, payload)

    def walk_pixel(self):

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
            NumberOfPixelUpdates=len(pixels),
            PixelData=pixels
        )

        header = mt.Header(
            MessageDataType=mt.DisplayEnums.PacketType.DISPLAY_INPUT,
            MessageDataSize=len(payload)
        )

        self.send_packet(header, payload)


    def update(self):

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

        payloadCommandBrightness = mt.Command(
            Command=mt.DisplayEnums.Command.BRIGHTNESS,
            Value=30
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
        #sock.sendto(str(headerCommand / payloadCommandBrightness), (self.dest, self.port))

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



def main():


    print("Pixel Display python side")

    sender = DisplayDataSender("192.168.1.110", 4210)

    #sender.update()

    while True:
        sender.walk_pixel()
        sleep(0.03)

    sender.clear()

    #for i in range(0, 3):
    #    sender.sendDisplayUpdate()
    #    sleep(5)

if __name__ == "__main__":
    main()