/*
*  Display
*
*  Documentation on the neopixel at:
*  https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.h
*/
#pragma once

#include <vector>

#include <Adafruit_NeoPixel.h>

// Other thirs party libraries
#include "mutex.h"                // https://github.com/raburton/esp8266

#include "config.h"

//! Display
//! Configuration in Config.h
//! The display uses the Adafruit NeoPixel library to draw
//! First set state of the display, then draw
class Display : public Adafruit_NeoPixel
{
public:
    Display();

    enum Mode {
        NORMAL = 0,
        CONNECTING,
        CLOCK,
        PULSE,
        GO_AROUND,
    };

    enum Effect {
        NONE = 0,
        FADE,
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
        uint8_t index;
        int r, g, b;

        Pixel()
        : index(0)
        , r(0)
        , g(0)
        , b(0)
        {}

        Pixel(uint8_t index_)
        : index(index_)
        , r(0)
        , g(0)
        , b(0)
        {}

        Pixel(uint8_t index_, int r_, int g_, int b_)
        : index(index_)
        , r(r_)
        , g(g_)
        , b(b_)
        {}

        Pixel operator+=(const Pixel& rhs)
        {
            return Pixel(index, rhs.r, rhs.g, rhs.b);
        }

        //! Implemented for use with sorting
        bool operator==(const Pixel& rhs)
        {
            return index == rhs.index;
        }

        bool operator!=(const Pixel& rhs)
        {
            return index != rhs.index ||
                   r != rhs.r ||
                   g != rhs.g ||
                   b != rhs.b;
        }

        //! Implemented for use with sorting
        bool operator<(const Pixel& rhs)
        {
            return index < rhs.index;
        }

        Pixel operator/(const Pixel& rhs)
        {
            return Pixel(index,
                         floor(r / (rhs.r == 0) ? 1 : rhs.r),
                         floor(g / (rhs.g == 0) ? 1 : rhs.g),
                         floor(b / (rhs.b == 0) ? 1 : rhs.b));
        }

        Pixel operator*(const Pixel& rhs)
        {
            return Pixel(index,
                         floor(r * rhs.r),
                         floor(g * rhs.g),
                         floor(b * rhs.b));
        }

        Pixel operator/(float d)
        {
            return Pixel(index,
                         round((float)r / d),
                         round((float)g / d), 
                         round((float)b / d));
        }

        Pixel operator*(float d)
        {
            return Pixel(index,
                         round((float)r * d),
                         round((float)g * d), 
                         round((float)b * d));
        }

        Pixel operator-(const Pixel& rhs)
        {
            return Pixel(index,
                         r - rhs.r,
                         g - rhs.g,
                         b - rhs.b);
        }


        Pixel operator+(const Pixel& rhs)
        {
            return Pixel(index,
                         r + rhs.r,
                         g + rhs.g,
                         b + rhs.b);
        }

    } __attribute__ ((__packed__));

    typedef std::vector<Pixel> PixelVec;

    void begin();

    //! Sets the Display mode and
    //! stores the new mode in EEPROM
    //! @mode: The new mode
    void setMode(Mode mode);

    Mode getMode();

    void setEffect(Effect effect);

    Effect getEffect();

    //! After writing to the display this will actuate / draw
    //! Return: display fully updated. e.g. with "FADE" active
    //!         it will take a few iterations
    bool draw();

    //! @b: Brightness in range [0, 255]
    void setBrightness(const uint8_t b);

    //! Clear set layer. "draw()" needs
    //! to be ran afterwards to execute
    //! @layer: Layer to clear
    void clear(Layer layer = Layer::MIDLAYER);

    void test();

    void disco();

    void setPixel(Pixel pixel, 
                  Layer layer = Layer::MIDLAYER);

    //! Set the color of a pixel in set layer
    void setPixel(uint8_t x, 
                  uint8_t y, 
                  uint32_t color, 
                  Layer layer = Layer::MIDLAYER);

    void setPixel(uint8_t x, 
                  uint8_t y, 
                  uint8_t r, 
                  uint8_t g, 
                  uint8_t b, 
                  Layer layer = Layer::MIDLAYER);

    void setPixels(std::vector<Pixel> pixels,
                  Layer layer = Layer::MIDLAYER,
                  Update update = Update::FULL);

    void setRow(uint8_t x, 
                uint32_t color, 
                Layer layer = Layer::MIDLAYER,
                Update update = Update::PARTIAL);

    void setRow(uint8_t x, 
                uint8_t r, 
                uint8_t g, 
                uint8_t b, 
                Layer layer = Layer::MIDLAYER,
                Update update = Update::PARTIAL);

private:
   
    typedef PixelVec::iterator PixelVecIter;
    typedef std::vector< PixelVec > PixelVecVec;
    typedef PixelVecVec::iterator LayersIter;

    float m_brightness;

    // TODO: Rename m_layers to current state?
    PixelVecVec m_layers;
    PixelVecVec m_layersGoalState;

    Mode m_mode;
    Effect m_effect;

    uint8_t m_width, m_height;

    mutex_t m_mutexDraw;

    //! @x: 0 indexed x coordinate
    //! @y: 0 indexed y coordinate
    //! Return: The index of the pixel corresponding to x, y
    uint8_t getPixelFromXY(uint8_t x, uint8_t y);

    Pixel pixelFromPackedColor(uint8_t index, uint32_t color);

    //! Clear a vector by setting default values (Keeps its size)
    //! Creating our own function as the STD library is not
    //! stable enough
    void clearVec(PixelVec& pixelVec);

    //! Because of approximations two pixel values may not be perfectly equal.
    //! This function evaluates if they are "close enough"
    //! @a: lhs pixel
    //! @b: rhs pixel
    //! Return: true if they are "equal"
    const bool almostEqualPixels(const Pixel& a, const Pixel& b) const;

};