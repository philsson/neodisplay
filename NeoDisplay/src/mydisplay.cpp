#include "mydisplay.h"


MyDisplay::MyDisplay()
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
: Adafruit_NeoPixel(NUM_PIXELS, PIN_PIXELS, NEO_GRB + NEO_KHZ800)
, m_brightness(100)
, m_layers(3, MyDisplay::PixelVec(NUM_PIXELS, Pixel()))
, m_layersGoalState(3, MyDisplay::PixelVec(NUM_PIXELS, Pixel()))
, m_layerBrightness(3, 255)
, m_mode(CONNECTING)
, m_effect(FADE)
, m_width(WIDTH)
, m_height(HEIGHT)
, m_mutexDraw()
{
    setBrightness(m_brightness);
    CreateMutex(&m_mutexDraw);
}

void MyDisplay::begin()
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

void MyDisplay::setMode(Mode mode)
{
    m_mode = mode;
}

MyDisplay::Mode MyDisplay::getMode()
{
    return m_mode;
}

void MyDisplay::setEffect(Effect effect, bool permanent)
{
    m_effect = effect;
    if (permanent)
    {
        Settings::Instance()->display.effect = effect;
        Settings::Instance()->save();
    }
}

MyDisplay::Effect MyDisplay::getEffect()
{
    return m_effect;
}

bool MyDisplay::draw()
{
    if (!GetMutex(&m_mutexDraw))
    {
        return false;
    }
    static const float d = 0.06f; // Update rate in range [0,1]
    bool fullyActuated = true;
    for (unsigned int i = 0; i < m_layers.size() /* same size as m_layersGoalState.size() */; i++)
    {
        float layerBrightness = float(m_layerBrightness[i])/255.0;

        for (unsigned int k = 0; k < m_layers[i].size(); k++)
        {
            Pixel& currentState = m_layers[i][k];
            Pixel& goalState = m_layersGoalState[i][k];
            goalState = goalState * layerBrightness;
            
            if (currentState != goalState)
            {
                fullyActuated = false;

                if (m_effect == FADE)
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

void MyDisplay::setBrightness(const uint8_t b)
{
    m_brightness = constrain(b, 0, MAX_BRIGHTNESS);
    Adafruit_NeoPixel::setBrightness(b);
}

void MyDisplay::setLayerBrightness(const uint8_t layer, const uint8_t b)
{
    m_layerBrightness[layer] = b;
}

void MyDisplay::restoreLayerBrightness(const MyDisplay::Layer layer)
{
    if (layer == MyDisplay::Layer::ALL)
    {
        for (int i = 0; i < 3; i++)
        {
            m_layerBrightness[i] = 255;
        }
    }
    else
    {
        m_layerBrightness[layer] = 255;
    }
}

void MyDisplay::clear(MyDisplay::Layer layer)
{
    if (layer == Layer::ALL)
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

void MyDisplay::test()
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

    for(int i = 0;; i++)
    {
        if (draw())
        {
            break;
        }
        if (i >= 100)
        {
            Serial.println("Timeout!. Breaking...");
            break;
        }
        delay(t);
    };

    clear();
}

void MyDisplay::disco()
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

void MyDisplay::setPixel(MyDisplay::Pixel pixel, 
                       MyDisplay::Layer layer)
{
    if (pixel.index <= NUM_PIXELS && layer != Layer::ALL)
    {
        m_layersGoalState[layer][pixel.index] = pixel;
    }
}

void MyDisplay::setPixel(uint8_t x, 
                       uint8_t y, 
                       uint32_t color, 
                       MyDisplay::Layer layer)
{
    uint8_t index = getPixelFromXY(x, y);
    setPixel(pixelFromPackedColor((uint8_t)index, color), layer);
}

void MyDisplay::setPixel(uint8_t x, 
                       uint8_t y, 
                       uint8_t r, 
                       uint8_t g, 
                       uint8_t b, 
                       MyDisplay::Layer layer)
{
    int index = getPixelFromXY(x, y);
    setPixel(MyDisplay::Pixel(index, r, g, b), layer);
}

void MyDisplay::setPixels(MyDisplay::PixelVec pixels, 
                        MyDisplay::Layer layer, 
                        MyDisplay::Update update)
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

void MyDisplay::setRow(uint8_t x, 
                     uint32_t color, 
                     MyDisplay::Layer layer,
                     MyDisplay::Update update)
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

void MyDisplay::setRow(uint8_t x, 
                     uint8_t r, 
                     uint8_t g, 
                     uint8_t b, 
                     MyDisplay::Layer layer,
                     MyDisplay::Update update)
{
    setRow(x, Adafruit_NeoPixel::Color(r, g, b), layer, update);
}

uint8_t MyDisplay::getPixelFromXY(uint8_t x, uint8_t y)
{
    if (UPPER_LEFT == 0)
    {
        uint8_t offsetY = (x % 2 == 0 ? y : LOWER_LEFT - y);
        return constrain(x * (LOWER_LEFT + 1) + offsetY, 0, NUM_PIXELS - 1);
    }
    return 0;
}

MyDisplay::Pixel MyDisplay::pixelFromPackedColor(uint8_t index, uint32_t color)
{
    return MyDisplay::Pixel(index,
                         (uint8_t)(color >> 16),
                         (uint8_t)(color >>  8),
                         (uint8_t)color);
}

void MyDisplay::clearVec(PixelVec& pixelVec)
{
    // This should have worked?
    // pixelVec.assign(pixelVec.size(), Pixel());

    for (PixelVecIter iter = pixelVec.begin(); iter != pixelVec.end(); iter++)
    {   
        *iter = Pixel(iter->index);
    }
}

const bool MyDisplay::almostEqualPixels(const Pixel& a, const Pixel& b) const
{
    // TODO: For some reason this condition does not seem satisfied for all cases 
    //       even after a long time. For example in "disco()" where if using this
    //       result as a condition we stay in an endless loop
    static const uint8_t t = 10;
    if (abs(a.r - b.r) > t || abs(a.g - b.g) > t || abs(a.b - b.b) > t  )
    {
        return false;
    }
    return true;
}