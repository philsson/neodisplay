#include "display.h"


Display::Display()
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
: m_strip(Adafruit_NeoPixel(NUM_PIXELS, PIN_PIXELS, NEO_GRB + NEO_KHZ800))
, m_brightness(100)
, m_pixels(NUM_PIXELS, Display::Pixel())
, m_width(WIDTH)
, m_height(HEIGHT)
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
    m_brightness = constrain(b, 0, MAX_BRIGHTNESS);
    m_strip.setBrightness(b);
    update();
}

void Display::clear()
{
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        m_pixels[i] = Pixel(i, 0, 0, 0);
        m_strip.setPixelColor(i, 0, 0, 0);
    }
    update();
}

void Display::test()
{
    uint8_t t = 50;
    int x;
    for (x = 0; x < m_width; x++)
    {
        setRowColor(x, m_strip.Color(255, 0, 0));
        update();
        delay(t);
    }

    for (x = m_width - 1; x >= 0; x--)
    {
        setRowColor(x, m_strip.Color(0, 255, 0));
        update();
        delay(t);
    }

    for (x = 0; x < m_width; x++)
    {
        setRowColor(x, m_strip.Color(0, 0, 255));
        update();
        delay(t);
    }

    for (x = m_width - 1; x >= 0; x--)
    {
        setRowColor(x, m_strip.Color(0, 0, 0));
        update();
        delay(t);
    }
}

void Display::disco()
{
  std::vector<Display::Pixel> pixels(NUM_PIXELS);
  for (int k = 0; k < 100; k++)
  {
    for (int i = 0; i < NUM_PIXELS; i++)
    {  
        pixels[i].index = i;
        pixels[i].r = random(0, 255);
        pixels[i].g = random(0, 255);
        pixels[i].b = random(0, 255);
    }
    setPixels(pixels);
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

uint8_t Display::getPixelFromXY(uint8_t x, uint8_t y)
{
    int index = 0;
    if (UPPER_LEFT == 0)
    {
        uint8_t offsetY = (x % 2 == 0 ? y : LOWER_LEFT - y);
        return constrain(x * (LOWER_LEFT + 1) + offsetY, 0, NUM_PIXELS - 1);
    }
}

void Display::setPixelColor(uint8_t x, uint8_t y, uint32_t color)
{
    int index = getPixelFromXY(x, y);
    m_strip.setPixelColor(index, color);
}

void Display::setPixelColor(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b)
{
    setPixelColor(x, y, m_strip.Color(r, g, b));
}

void Display::setRowColor(uint8_t x, uint32_t color)
{
    for (int y = 0; y < m_height; y++)
    {
        int index = getPixelFromXY(x, y);
        m_strip.setPixelColor(index, color);
    }
}

void Display::setRowColor(uint8_t x, uint8_t r, uint8_t g, uint8_t b)
{
    setRowColor(x, m_strip.Color(r, g, b));
}