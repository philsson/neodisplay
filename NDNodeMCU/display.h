/*
*  Display
*/
#pragma once

#include <vector>

#include <Adafruit_NeoPixel.h>

#include "config.h"

enum Mode {
    CLOCK = 0,
    PULSE,
    GO_AROUND,
};

class Display
{
public:
    Display();

    // Content of one pixel
    typedef struct Pixel {
        uint8_t index, r, g, b;

        Pixel()
        : index(0)
        , r(0)
        , g(0)
        , b(0)
        {}

        Pixel(uint8_t index_, uint8_t r_, uint8_t g_, uint8_t b_)
        : index(index_)
        , r(r_)
        , g(g_)
        , b(b_)
        {}

    } __attribute__ ((__packed__));

    void begin();

    // After writing to the display this will actuate
    void update();

    // value between 0-1
    void setBrightness(const uint8_t b);

    void clear();

    void test();

    void disco();

    void setPixel(Pixel pixel);

    // TODO: Set to use "type of update"
    void setPixels(std::vector<Pixel> pixels);

    void setMode(Mode mode);

    Mode getMode();

private:
    Adafruit_NeoPixel m_strip;

    float m_brightness;

    std::vector<Pixel> m_pixels;

    Mode m_mode;

    uint8_t m_width, m_height;

    // x: 0 indexed x coordinate
    // y: 0 indexed y coordinate
    // Return: The index of the pixel corresponding to x, y
    uint8_t getPixelFromXY(uint8_t x, uint8_t y);

    void setRowColor(uint8_t x, uint32_t color);
};