#include <Arduino.h>
#include "graphics.h"

using namespace fonts::std;
using namespace fonts::drawings;

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
                display.setPixel(xPos + x, yPos + y, color, Display::MIDLAYER);
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
            display.setPixel(xPos, y, color, Display::MIDLAYER);
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

void plotArr(Display& display,
             const byte arr[],
             uint8_t width, 
             uint8_t height,
             uint8_t xPos, 
             uint8_t yPos, 
             uint32_t color,
             Display::Layer layer)
{
    static const uint8_t xOffset = sizeof(byte) - width;
    
    for (uint8_t y = 0; y < height; y++)
    {
        byte row = arr[y];
        for (uint8_t x = 0; x < width; x++)
        {
            if (row >> (width - 1 - x) & 0x01)
            {
                display.setPixel(xPos + x, yPos + y, color, layer);
            }
        }
    }
}

bool plotWiFi(Display& display, bool doneConnecting, bool connected)
{
    // TODO: Write the status symbol (Green Tick and Red X)
    static const Display::Layer layer = Display::Layer::MIDLAYER;
    display.clear(layer);
    //display.clear(Display::Layer::ALL);
    static uint8_t turn = 0;
    static int tick = 0;
    static const int animationTime = 1000; // Time for the whole symbol to update / pulse

    // Plot dot
    plotArr(display, fonts::drawings::wifi[3].d, 2, 8, 21, 0, 0x1E90FF, layer);

    if (turn == 1)
    {
        // Plot first bar
        plotArr(display, fonts::drawings::wifi[2].d, 7, 8, 22, 0, 0x1E90FF, layer);
    }

    if (turn == 2)
    {
        // Plot first bar
        plotArr(display, fonts::drawings::wifi[1].d, 7, 8, 22, 0, 0x1E90FF, layer);
    }

    if (turn == 3)
    {
        // Plot first bar
        plotArr(display, fonts::drawings::wifi[0].d, 7, 8, 22, 0, 0x1E90FF, layer);
    }

    if (doneConnecting)
    {
        if (connected)
        {
            display.setPixel(4, 4, 0x00FF00, layer);
        }
        else
        {
            display.setPixel(4, 4, 0xFF0000, layer);
        }
    }
    tick++;
    if (tick >= animationTime/(LOOPTIME*3))
    {
        turn++;
        if (turn == 4)
        {
            turn = 0;
            return true;
        }
        tick = 0;
    }
    return false;
}