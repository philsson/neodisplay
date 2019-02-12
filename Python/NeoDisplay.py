import argparse

from DisplayDataSender import DisplayDataSender
from Snake import Snake
from Display import Display

from pythonosc import dispatcher
from pythonosc import osc_server

import pythonosc as posc

import mt


def print_topic(path, arg1=None, arg2=None, arg3=None):
    """
    Testfunction for OSC topics
    """
    print("Path:", path)
    if arg1 is not None:
        print("Arg1:", arg1)
    if arg2 is not None:
        print("Arg2:", arg2)
    if arg3 is not None:
        print("Arg3:", arg3)


def main():
    # UDP Sender to NodeMCU on NeoDisplay
    sender = DisplayDataSender("NeoDisplay", 4210)

    # The game
    snake = Snake(Display.WIDTH, Display.HEIGHT, sender.update)

    # OSC Server configuration
    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", default="0.0.0.0", help="The ip to listen on")
    parser.add_argument("--port", type=int, default=8888, help="The port to listen on")
    args = parser.parse_args()
    dispatcher = posc.dispatcher.Dispatcher()
    #dispatcher.map("/*", print_topic) # Wildcard for all topics
    dispatcher.map("/Snake/Dir/*", snake.turn)
    dispatcher.map("/Snake/Reset", snake.reset)
    dispatcher.map("/Snake/Fader", snake.speed)
    server = posc.osc_server.ThreadingOSCUDPServer((args.ip, args.port), dispatcher)

    sender.set_brightness(35)
    sender.set_mode(mt.DisplayEnums.Mode.DEFAULT)
    sender.clear(mt.DisplayEnums.Layer.ALL)
    sender.set_effect(mt.DisplayEnums.Effect.NONE)

    # Start Stuff:
    print("Serving OSC on {}".format(server.server_address))
    server.serve_forever()

    # Ending Stuff
    server.close


if __name__ == "__main__":
    main()
