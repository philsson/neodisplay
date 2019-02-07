#include "display.h"


Display::Display()
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
: Adafruit_NeoPixel(NUM_PIXELS, PIN_PIXELS, NEO_GRB + NEO_KHZ800)
, m_brightness(100)
, m_layers(3, Display::PixelVec(NUM_PIXELS, Pixel()))
, m_layersGoalState(3, Display::PixelVec(NUM_PIXELS, Pixel()))
, m_mode(CONNECTING)
, m_effect(FADE)
, m_width(WIDTH)
, m_height(HEIGHT)
, m_mutexDraw()
{
    setBrightness(m_brightness);
    CreateMutex(&m_mutexDraw);
}

void Display::begin()
{
    for (LayersIter lIter = m_layers.begin(); lIter != m_layers.end(); lIter++)
    {
        int i = 0;
        for (PixelVecIter vIter = lIter->begin(); vIter != lIter->end(); vIter++)
        {
            vIter->index = i++;
        }
    }
    for (LayersIter lIter = m_layersGoalState.begin(); lIter != m_layersGoalState.end(); lIter++)
    {
        int i = 0;
        for (PixelVecIter vIter = lIter->begin(); vIter != lIter->end(); vIter++)
        {
            vIter->index = i++;
        }
    }

    Adafruit_NeoPixel::begin();
}

void Display::setMode(Mode mode)
{
    m_mode = mode;
    config.display.mode = mode;
    config.save();
}

Display::Mode Display::getMode()
{
    return m_mode;
}

bool Display::draw()
{
    static const float d = 0.08f;
    if (!GetMutex(&m_mutexDraw))
    {
        return false;
    }
    bool fullyActuated = true;
    for (int i = 0; i < m_layers.size() /* and m_layersGoalState.size() */; i++)
    {
        for (int k = 0; k < m_layers[i].size(); k++)
        {
            Pixel& currentState = m_layers[i][k];
            Pixel& goalState = m_layersGoalState[i][k];
            
            if (currentState != goalState)
            {
                fullyActuated = false;

                if (m_mode == FADE)
                {
                    if (almostEqualPixels(currentState, goalState))
                    {
                        currentState = goalState;
                    }
                    else
                    {
                        currentState = currentState*(1.0f-d) + goalState*d;
                    }
                }
                else
                {
                    currentState = goalState;
                }
            }    
        }
    }

    PixelVec pixels(NUM_PIXELS, Pixel());

    Adafruit_NeoPixel::clear();
    for (LayersIter layerIt = m_layers.begin(); layerIt != m_layers.end(); layerIt++)
    {
        for (PixelVecIter it = layerIt->begin(); it != layerIt->end(); it++)
        {
            if (it->r | it->g | it->b)
            {   // Don't plot (Or erase if higher layer) when all colors are 0
                pixels[it->index] = *it;
            }
        }
    }
    for (PixelVecIter iter = pixels.begin(); iter != pixels.end(); iter++)
    {
        if (iter->r | iter->g | iter->b)
        {
            Adafruit_NeoPixel::setPixelColor(iter->index, iter->r, iter->g, iter->b);
        }
    }
    Adafruit_NeoPixel::show();
    ReleaseMutex(&m_mutexDraw);
    return fullyActuated;
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
        for (LayersIter layerIt = m_layersGoalState.begin(); layerIt != m_layersGoalState.end(); layerIt++)
        {
            clearVec(*layerIt);
        }
    }
    else
    {
        clearVec(m_layersGoalState[layer]);
    }
    Adafruit_NeoPixel::clear();
}

void Display::test()
{
    static const uint8_t t = 1000/LOOPTIME;
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

    for(int i = 0; i < 100; i++)
    {
        if (draw())
        {
            break;
        }
        delay(t);
    };

    clear();
}

void Display::disco()
{
  static const uint8_t t = 1000/LOOPTIME;

  PixelVec pixels(NUM_PIXELS);
  for (int k = 0; k < 10; k++)
  {
    int i = 0;
    for (PixelVecIter iter = pixels.begin(); iter != pixels.end(); iter++)
    {  
        iter->index = i++;
        iter->r = random(0, 255);
        iter->g = random(0, 255);
        iter->b = random(0, 255);
    }
    setPixels(pixels);

    for(int i = 0; i < 10; i++)
    {
        if (draw())
        {
            break;
        }
        delay(t);
    };
  }
}

void Display::setPixel(Display::Pixel pixel, 
                       Display::Layer layer)
{
    if (pixel.index <= NUM_PIXELS && layer != ALL)
    {
        m_layersGoalState[layer][pixel.index] = pixel;
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
        clearVec(m_layersGoalState[layer]);
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
        clearVec(m_layersGoalState[layer]);
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

void Display::clearVec(PixelVec& pixelVec)
{
    // This should have worked?
    // pixelVec.assign(pixelVec.size(), Pixel());

    for (PixelVecIter iter = pixelVec.begin(); iter != pixelVec.end(); iter++)
    {   
        *iter = Pixel(iter->index);
    }
}

const bool Display::almostEqualPixels(const Pixel& a, const Pixel& b) const
{
    // TODO: For some reason this condition does not seem satisfied for all cases 
    //       even after a long time. For example in "disco()" where if using this
    //       result as a condition we stay in an endless loop
    static const uint8_t t = 2;
    if (abs(a.r - b.r) > t || abs(a.g - b.g) > t || abs(a.b - b.b) > t  )
    {
        return false;
    }
    return true;
}