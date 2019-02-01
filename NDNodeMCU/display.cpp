#include "display.h"


Display::Display()
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
: Adafruit_NeoPixel(NUM_PIXELS, PIN_PIXELS, NEO_GRB + NEO_KHZ800)
, m_brightness(100)
, m_layers(3, Display::PixelVec())
, m_width(WIDTH)
, m_height(HEIGHT)
{
    setBrightness(m_brightness);
}

void Display::begin()
{
    Adafruit_NeoPixel::begin();
}

void Display::setMode(Mode mode)
{
    m_mode = mode;
    // TODO: Some other stuff
}

Display::Mode Display::getMode()
{
    return m_mode;
}

void Display::draw()
{
    // TODO: Optimize by calculating the hole display beforehand. Avoiding
    //       multiple writes to same pixel before plotting

    Adafruit_NeoPixel::clear();
    for (LayersIter layerIt = m_layers.begin(); layerIt != m_layers.end(); layerIt++)
    {
        for (PixelVecIter it = layerIt->begin(); it != layerIt->end(); it++)
        {
            if (it->r | it->g | it->b)
            {   // Don't plot (Or erase if higher layer) when all colors are 0
                Adafruit_NeoPixel::setPixelColor(it->index, it->r, it->g, it->b);
            }
        }
    }
    Adafruit_NeoPixel::show();
}

void Display::setBrightness(const uint8_t b)
{
    m_brightness = constrain(b, 0, MAX_BRIGHTNESS);
    Adafruit_NeoPixel::setBrightness(b);
}

void Display::clear(Display::Layer layer)
{
    if (layer == ALL)
    {
        for (LayersIter layerIt = m_layers.begin(); layerIt != m_layers.end(); layerIt++)
        {
            layerIt->clear();
        }
    }
    else
    {
        m_layers[layer].clear();
    }
    Adafruit_NeoPixel::clear();
    draw();
}

void Display::test()
{
    uint8_t t = 50;
    int x;
    for (x = 0; x < m_width; x++)
    {
        setRow(x, Adafruit_NeoPixel::Color(255, 0, 0));
        draw();
        delay(t);
    }

    for (x = m_width - 1; x >= 0; x--)
    {
        setRow(x, Adafruit_NeoPixel::Color(0, 255, 0));
        draw();
        delay(t);
    }

    for (x = 0; x < m_width; x++)
    {
        setRow(x, Adafruit_NeoPixel::Color(0, 0, 255));
        draw();
        delay(t);
    }

    for (x = m_width - 1; x >= 0; x--)
    {
        setRow(x, Adafruit_NeoPixel::Color(0, 0, 0));
        draw();
        delay(t);
    }

    clear();
}

void Display::disco()
{
  PixelVec pixels(NUM_PIXELS);
  for (int k = 0; k < 10; k++)
  {
    for (int i = 0; i < NUM_PIXELS; i++)
    {  
        pixels[i].index = i;
        pixels[i].r = random(0, 255);
        pixels[i].g = random(0, 255);
        pixels[i].b = random(0, 255);
    }
    setPixels(pixels);
    draw();
    delay(100);
  }
}

void Display::setPixel(Display::Pixel pixel, 
                       Display::Layer layer)
{
    if (pixel.index <= NUM_PIXELS)
    {
        if (layer != ALL)
        {
            for (PixelVecIter it = m_layers[layer].begin(); it != m_layers[layer].end(); it++)
            {
                if (it->index == pixel.index)
                {
                    *it = pixel;
                    return;
                }
            }
            m_layers[layer].push_back(pixel);
        }
    }
}

void Display::setPixel(uint8_t x, 
                       uint8_t y, 
                       uint32_t color, 
                       Display::Layer layer)
{
    uint8_t index = getPixelFromXY(x, y);
    setPixel(pixelFromPackedColor((uint8_t)index, color), layer);
}

void Display::setPixel(uint8_t x, 
                       uint8_t y, 
                       uint8_t r, 
                       uint8_t g, 
                       uint8_t b, 
                       Display::Layer layer)
{
    int index = getPixelFromXY(x, y);
    setPixel(Display::Pixel(index, r, g, b), layer);
}

void Display::setPixels(Display::PixelVec pixels, 
                        Display::Layer layer, 
                        Display::Update update)
{
    if (update == FULL)
    {
        m_layers[layer].clear();
    }
    for (PixelVecIter it = pixels.begin(); it != pixels.end(); it++)
    {        
        setPixel(*it, layer);
    }
}

void Display::setRow(uint8_t x, 
                     uint32_t color, 
                     Display::Layer layer,
                     Display::Update update)
{
    if (update == FULL)
    {
        m_layers[layer].clear();
    }
    for (int y = 0; y < m_height; y++)
    {
        int index = getPixelFromXY(x, y);
        setPixel(pixelFromPackedColor(index, color), layer);
    }
}

void Display::setRow(uint8_t x, 
                     uint8_t r, 
                     uint8_t g, 
                     uint8_t b, 
                     Display::Layer layer,
                     Display::Update update)
{
    setRow(x, Adafruit_NeoPixel::Color(r, g, b), layer, update);
}

uint8_t Display::getPixelFromXY(uint8_t x, uint8_t y)
{
    int index = 0;
    if (UPPER_LEFT == 0)
    {
        uint8_t offsetY = (x % 2 == 0 ? y : LOWER_LEFT - y);
        return constrain(x * (LOWER_LEFT + 1) + offsetY, 0, NUM_PIXELS - 1);
    }
    return 0;
}

Display::Pixel Display::pixelFromPackedColor(uint8_t index, uint32_t color)
{
    return Display::Pixel(index,
                         (uint8_t)(color >> 16),
                         (uint8_t)(color >>  8),
                         (uint8_t)color);
}