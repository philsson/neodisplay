#include "display.h"

#define NUM_PIXELS 256
#define PIN_PIXELS 14

Display::Display()
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
: m_strip(Adafruit_NeoPixel(NUM_PIXELS, PIN_PIXELS, NEO_GRB + NEO_KHZ800))
, m_brightness(100)
, m_pixels(NUM_PIXELS, Display::Pixel())
{
    setBrightness(m_brightness);
}

void Display::begin()
{
    m_strip.begin();
}

void Display::update()
{
    m_strip.show();
}

void Display::setBrightness(const uint8_t b)
{
    m_brightness = b;
    m_strip.setBrightness(b);
    update();
}

void Display::clear()
{
    m_pixels.clear();
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        m_strip.setPixelColor(i, 0, 0, 0);
    }
    update();
}

void Display::test()
{
    int i;
    for (i = 0; i < NUM_PIXELS; i++)
    {
        m_strip.setPixelColor(i, m_strip.Color(255, 0, 0));
        m_strip.show();
        delay(10);
    }

    for (i = NUM_PIXELS - 1; i >= 0; i--)
    {
        m_strip.setPixelColor(i, m_strip.Color(0, 255, 0));
        m_strip.show();
        delay(10);
    }

    for (i = 0; i < NUM_PIXELS; i++)
    {
        m_strip.setPixelColor(i, m_strip.Color(0, 0, 255));
        m_strip.show();
        delay(10);
    }

    for (i = NUM_PIXELS - 1; i >= 0; i--)
    {
        m_strip.setPixelColor(i, m_strip.Color(0, 0, 0));
        m_strip.show();
        delay(10);
    }
}

void Display::setPixel(Display::Pixel pixel)
{
    if (pixel.index <= NUM_PIXELS)
    {
        m_pixels[pixel.index] = pixel;
        m_strip.setPixelColor(pixel.index, pixel.r, pixel.g, pixel.b);
    }
}

void Display::setPixels(std::vector<Display::Pixel> pixels)
{
    for (std::vector<Pixel>::iterator it = pixels.begin(); it != pixels.end(); it++)
    {        
        setPixel(*it);
    }
    update();
}

void Display::setMode(Mode mode)
{
    m_mode = mode;
    // TODO: Some other stuff
}

Mode Display::getMode()
{
    return m_mode;
}