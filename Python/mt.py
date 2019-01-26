import scapy.all as scapy
from enum import Enum

"""
Little Endian (LE) types are needed when received on NodeMCU side 
IF they are to be stored directly into a datatype larger than 1 byte
or into a packed struct

Just append LE
"""


class DisplayEnums:

    class PacketType:
        MODE = 0
        COMMAND = 1
        DISPLAY_INPUT = 2

    class Mode:
        CLOCK = 0
        PULSE = 1
        GO_AROUND = 2

    class Command:
        CLEAR = 0
        BRIGHTNESS = 1
        TEST = 2

    class DisplayUpdate:
        PARTIAL = 0
        FULL = 1

    class Layer:
        BACKGROUND = 0
        MIDLAYER = 1
        FOREGROUND = 2


# Header
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


class Command(scapy.Packet):
    name = "Command"
    fields_desc = [
        scapy.ByteField("Command", 0x00),
        scapy.ByteField("Value", 0x00)
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
        scapy.FieldLenField("NumberOfPixelUpdates", None, count_of="PixelData", fmt='H'),
        scapy.PacketListField("PixelData", None, Pixel, count_from=lambda pkt: pkt.NumberOfPixelUpdates)
    ]