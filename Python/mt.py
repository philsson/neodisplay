import scapy.all as scapy

"""
Little Endian (LE) types are needed when received on NodeMCU side 
IF they are to be stored directly into a datatype larger than 1 byte
or into a packed struct

Just append LE
"""


class DisplayEnums:

    class PacketType:
        MODE = 0
        EFFECT = 1
        COMMAND = 2
        DISPLAY_INPUT = 3

    class Mode:
        DEFAULT = 0
        CONNECTING = 1
        CLOCK = 2
        PULSE = 3
        GO_AROUND = 4

    class Effect:
        NONE = 0
        FADE = 1

    class Command:
        CLEAR = 0
        BRIGHTNESS = 1
        LAYER_BRIGHTNESS = 2
        TEST = 3
        RENEW_TIME = 4
        RESET_WIFI = 5
        CONFIGURE_WIFI = 6

    class DisplayUpdate:
        PARTIAL = 0
        FULL = 1

    class Layer:
        BACKGROUND = 0
        MIDLAYER = 1
        FOREGROUND = 2
        ALL = 3


# Header - Followed by one of the payloads below
class Header(scapy.Packet):
    name = "Header"
    fields_desc = [
        scapy.IntField("MagicWord", 0xDEC0DED),  # 32 bit
        scapy.ByteField("MessageDataType", 0x00),  # 8 bit
        scapy.LEShortField("MessageDataSize", 0x00)  # 16 bit
    ]


class Mode(scapy.Packet):
    name = "Mode"
    fields_desc = [
        scapy.ByteField("Mode", 0x00)
    ]


class Effect(scapy.Packet):
    name = "Effect"
    fields_desc = [
        scapy.ByteField("Effect", 0x00),
        scapy.ByteField("Permanent", 0x00)
    ]


class Command(scapy.Packet):
    name = "Command"
    fields_desc = [
        scapy.ByteField("Command", 0x00),
        scapy.ByteField("Value", 0x00),
        scapy.ByteField("Value2", 0x00)
    ]


class Pixel(scapy.Packet):
    name = "Pixel"
    fields_desc = [
        scapy.ByteField("Index", 0x00),
        scapy.ByteField("Red", 0x00),
        scapy.ByteField("Green", 0x00),
        scapy.ByteField("Blue", 0x00)
    ]


class Pixels(scapy.Packet):
    name = "Pixels"
    fields_desc = [
        scapy.ByteField("TypeOfUpdate", 0x00),
        scapy.ByteField("Layer", 0x01),
        scapy.FieldLenField("NumberOfPixelUpdates", None, count_of="PixelData", fmt='H'),
        scapy.PacketListField("PixelData", None, Pixel, count_from=lambda pkt: pkt.NumberOfPixelUpdates)
    ]

