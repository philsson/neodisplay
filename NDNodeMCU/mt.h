/*
*  Data Protocols
*/
#pragma once

#include <Arduino.h>
#include <vector>
#include "display.h"
#include "clock.h"

enum PacketType {
    MODE = 0,
    EFFECT,
    COMMAND,
    DISPLAY_INPUT, // pixel updates
};

enum Command {
    CLEAR = 0, // Empty all but the permanent part
    BRIGHTNESS,
    LAYER_BRIGHTNESS,
    TEST,
    RENEW_TIME,
    RESET_WIFI,
    CONFIGURE_WIFI,
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

typedef struct PacketEffect {
    uint8_t effect;
} __attribute__ ((__packed__));

typedef struct PacketCommand {
    uint8_t command;
    uint8_t value;
    uint8_t second_value;
} __attribute__ ((__packed__));

typedef struct PacketDisplayUpdate {
    uint8_t typeOfUpdate;
    uint8_t layer;
    uint16_t numOfPixels;
    std::vector<Display::Pixel> pixels;
} __attribute__ ((__packed__));

class DisplayParser
{
public:
    DisplayParser(Display& display, Clock& clock);

    bool parse(const uint8_t byteIn);

private:

    bool parseMode(const int index, const int packetSize, const uint8_t byteIn);

    bool parseEffect(const int index, const int packetSize, const uint8_t byteIn);

    bool parseCommand(const int index, const int packetSize, const uint8_t byteIn);

    bool parseDisplay(const int index, const int packetSize, const uint8_t byteIn);

    void actuateCommand(Command command, const uint8_t value, const uint8_t value2);

    void actuateDisplay(DisplayUpdate update, const Display::PixelVec& pixels, Display::Layer layer);

    Display& m_display;

    Clock& m_clock;
};