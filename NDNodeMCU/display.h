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

    //! Content of one pixel
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

        bool operator==(const Pixel& rhs)
        {
            return index == rhs.index;
        }

        bool operator<(const Pixel& rhs)
        {
            return index < rhs.index;
        }

    } __attribute__ ((__packed__));

    void begin();

    //! After writing to the display this will actuate
    void update();

    //! @b: Brightness in range [0, 255]
    void setBrightness(const uint8_t b);

    void clear();

    void test();

    void disco();

    void setPixel(Pixel pixel);

    // TODO: Set to use "type of update"
    void setPixels(std::vector<Pixel> pixels);

    void setPixelColor(uint8_t x, uint8_t y, uint32_t color);

    void setPixelColor(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);

    void setRowColor(uint8_t x, uint32_t color);

    void setRowColor(uint8_t x, uint8_t r, uint8_t g, uint8_t b);

    void setMode(Mode mode);

    Mode getMode();

private:
    Adafruit_NeoPixel m_strip;

    float m_brightness;

    // State of this display. Do not change size
    std::vector<Pixel> m_pixels;

    Mode m_mode;

    uint8_t m_width, m_height;

    //! @x: 0 indexed x coordinate
    //! @y: 0 indexed y coordinate
    //! Return: The index of the pixel corresponding to x, y
    uint8_t getPixelFromXY(uint8_t x, uint8_t y);

    
};