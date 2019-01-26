/*
*  Display
*
*  Documentation on the neopixel at:
*  https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.h
*/
#pragma once

#include <vector>

#include <Adafruit_NeoPixel.h>

#include "config.h"

//! Display
//! Configuration in Config.h
//! The display uses the Adafruit NeoPixel library to draw
//! 
class Display : public Adafruit_NeoPixel
{
public:
    Display();

    enum Mode {
        CLOCK = 0,
        PULSE,
        GO_AROUND,
    };

    enum Layer {
        BACKGROUND = 0,
        MIDLAYER,
        FOREGROUND,
        
        ALL, // Keep "ALL" at bottom
    };

    enum Update {
        PARTIAL = 0,
        FULL,
    };

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

    typedef std::vector<Pixel> PixelVec;

    void begin();

    void setMode(Mode mode);

    Mode getMode();

    //! After writing to the display this will actuate / draw
    void draw();

    //! @b: Brightness in range [0, 255]
    void setBrightness(const uint8_t b);

    void clear(Layer layer = MIDLAYER);

    void test();

    void disco();

    void setPixel(Pixel pixel, 
                  Layer layer = MIDLAYER,
                  Update draw = PARTIAL);

    //! Set the state (color) of a pixel in set layer
    void setPixel(uint8_t x, 
                  uint8_t y, 
                  uint32_t color, 
                  Layer layer = MIDLAYER,
                  Update draw = PARTIAL);

    void setPixel(uint8_t x, 
                  uint8_t y, 
                  uint8_t r, 
                  uint8_t g, 
                  uint8_t b, 
                  Layer layer = MIDLAYER,
                  Update draw = PARTIAL);

    // TODO: Set to use "type of draw"

    void setPixels(std::vector<Pixel> pixels,
                  Layer layer = MIDLAYER,
                  Update draw = FULL);

    void setRow(uint8_t x, 
                uint32_t color, 
                Layer layer = MIDLAYER,
                Update draw = PARTIAL);

    void setRow(uint8_t x, 
                uint8_t r, 
                uint8_t g, 
                uint8_t b, 
                Layer layer = MIDLAYER,
                Update draw = PARTIAL);

private:
   
    typedef PixelVec::iterator PixelVecIter;
    typedef std::vector< PixelVec > PixelVecVec;
    typedef PixelVecVec::iterator LayersIter;

    float m_brightness;

    PixelVecVec m_layers;

    Mode m_mode;

    uint8_t m_width, m_height;

    //! @x: 0 indexed x coordinate
    //! @y: 0 indexed y coordinate
    //! Return: The index of the pixel corresponding to x, y
    uint8_t getPixelFromXY(uint8_t x, uint8_t y);

    Pixel pixelFromPackedColor(uint8_t index, uint32_t color);

};