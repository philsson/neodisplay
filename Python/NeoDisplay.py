from time import sleep


from OSC import OSCServer,OSCClient, OSCMessage
from time import sleep
import types

import DisplayDataSender


import mt



#
# def push_xy_callback(path, tags, args, source):
#     if path == "/1/xy1":
#         print("x:", args[0], " y:", args[1])

def handle_error(self, request, client_address):
    pass






def main():

    print("Pixel Display python side")

    sender = DisplayDataSender.DisplayDataSender("192.168.1.110", 4210)

    osc_server = OSCServer(("0.0.0.0", 8888))
    #server.addMsgHandler("/1/xy1", push_xy_callback)
    osc_server.addMsgHandler("/1/xy1", sender.slider)

    osc_server.handle_error = types.MethodType(handle_error, osc_server)

    #sender.send_command(mt.DisplayEnums.Command.CONFIGURE_WIFI)
    #sender.send_command(mt.DisplayEnums.Command.RESET_WIFI)
    sender.set_effect(mt.DisplayEnums.Effect.NONE)
    #sender.set_effect(mt.DisplayEnums.Effect.FADE)

    #return


    #while True:
    #    osc_server.handle_request()

    #sender.update()

    print("moving forward with walking pixel")
    
    while True:
        sender.walk_pixel()
        sleep(0.03)

    sender.clear()

    #for i in range(0, 3):
    #    sender.sendDisplayUpdate()
    #    sleep(5)

    server.close

if __name__ == "__main__":
    main()