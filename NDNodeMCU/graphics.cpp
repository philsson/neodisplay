#include <Arduino.h>
#include "graphics.h"

using namespace fonts::std;

void plotNum(Display& display, uint8_t num, uint8_t xPos, uint8_t yPos, uint32_t color)
{
    static const uint8_t xOffset = sizeof(byte) - FONT_WIDTH;
    
    for (uint8_t y = 0; y < FONT_HEIGHT; y++)
    {
        byte row = digits[num].d[y];
        for (uint8_t x = 0; x < FONT_WIDTH; x++)
        {
            if (row >> (FONT_WIDTH - 1 - x) & 0x01)
            {
                display.setPixel(xPos + x, yPos + y, color, Display::MIDLAYER, Display::PARTIAL);
            }
        }
    }
}

void plotColumn(Display& display, uint8_t column, uint8_t xPos, uint32_t color)
{
    for (uint8_t y = 0; y < min(8, FONT_HEIGHT); y++)
    {
        if (column >> (FONT_HEIGHT - 1 - y) & 0x01)
        {
            display.setPixel(xPos, y, color, Display::MIDLAYER, Display::PARTIAL);
        }
    }
}

void plotClock(const Clock& clock, Display& display)
{
    display.clear(Display::MIDLAYER);
    const Clock::MyTime time = clock.getTime();

    static const uint8_t separator = B01100110;

    // Hours in AQUAMARINE
    plotNum(display, time.hour / 10, 1, 0, 0x7FFFD4);
    plotNum(display, time.hour % 10, 6, 0, 0x7FFFD4);

    // Minutes in DODGERBLUE
    plotNum(display, time.minute / 10, 11, 0, 0x1E90FF);
    plotNum(display, time.minute % 10, 16, 0, 0x1E90FF);

    plotNum(display, time.second / 10, 21, 0, 0xFF6432);
    plotNum(display, time.second % 10, 26, 0, 0xFF6432);

    // Separators
    if (time.second % 2)
    {
        plotColumn(display, separator, 10, 0x00FF00);
        plotColumn(display, separator, 20, 0x00FF00); // 0x5600FF nice purple
    }
}