/*
*  Data Protocols
*/
#pragma once

#include <Arduino.h>
#include <vector>
#include "display.h"

enum PacketType {
    MODE = 0,
    COMMAND,
    DISPLAY_INPUT, // pixel updates
};

enum Command {
    CLEAR = 0, // Empty all but the permanent part
    BRIGHTNESS,
    TEST,
};

enum DisplayUpdate {
    PARTIAL = 0,
    FULL,
    PERMANENT, // What should be remembered
};

enum ParseMode {
    MAGICWORD = 0,
    REST_OF_HEADER,
    PAYLOAD,
};

typedef struct PacketHeader {
    int32_t magicWord;
    uint8_t packetType;
    uint16_t packetSize;

} __attribute__ ((__packed__));

typedef struct PacketMode {
    uint8_t mode;
} __attribute__ ((__packed__));

typedef struct PacketCommand {
    uint8_t command;
    uint8_t value;
} __attribute__ ((__packed__));

typedef struct PacketDisplayUpdate {
    uint8_t typeOfUpdate;
    uint16_t numOfPixels;
    std::vector<Display::Pixel> pixels;
} __attribute__ ((__packed__));

class DisplayParser
{
public:
    DisplayParser(Display& display);

    bool parse(const uint8_t byteIn);

private:

    bool parseMode(const int index, const int packetSize, const uint8_t byteIn);

    bool parseCommand(const int index, const int packetSize, const uint8_t byteIn);

    bool parseDisplay(const int index, const int packetSize, const uint8_t byteIn);

    void actuateCommand(Command command, const uint8_t value);

    void actuateDisplay(DisplayUpdate update, const Display::PixelVec& pixels);

    Display& m_display;
};