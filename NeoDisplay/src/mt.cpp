#include "mt.h"
#include "config.h"

DisplayParser::DisplayParser(MyDisplay& display, Clock& clock)
: m_display(display)
, m_clock(clock)
{}

bool DisplayParser::parse(const uint8_t byteIn)
{
    static const int headerSize = sizeof(PacketHeader);
    static uint8_t parsingMode = MAGICWORD;

    static uint32_t magicWord;

    static uint8_t header[headerSize];
    static PacketHeader* pHeader = (PacketHeader*)header;

    static int payloadParseIndex = 0;
    static int index = 0;

    bool tempBool = false;

    switch (parsingMode)
    {
    case MAGICWORD:
        magicWord = magicWord << 8 | byteIn;
        if (magicWord == 0xDEC0DED)
        {
            //Serial.printf("MAGICWORD\n");
            magicWord = 0;
            parsingMode = REST_OF_HEADER;
            index = 3;
        }
        break;

    case REST_OF_HEADER:
        
        //Serial.printf("Appending to header. Index = %d\n", index);
        header[index] = byteIn;
        if (index == headerSize - 1)
        {
            //Serial.printf("Done reading header\n");
            //Serial.printf("Packet type: %d\n", pHeader->packetType);
            //Serial.printf("Packet size: %d\n", pHeader->packetSize);
            parsingMode = PAYLOAD;
            payloadParseIndex = 0;
        }
        break;
    
    case PAYLOAD:

        //payloadArr[payloadParseIndex++] = byteIn;

        bool done = false;
        switch (pHeader->packetType)
        {
        case MODE:
            //Serial.printf("Unpacking MODE\n");
            done = parseMode(payloadParseIndex++, pHeader->packetSize, byteIn);
            break;

        case EFFECT:
            //Serial.printf("Unpacking MODE\n");
            done = parseEffect(payloadParseIndex++, pHeader->packetSize, byteIn);
            break;

        case COMMAND:
            //Serial.printf("Unpacking COMMAND\n");
            done = parseCommand(payloadParseIndex++, pHeader->packetSize, byteIn);
            break;

        case DISPLAY_INPUT:
            //Serial.printf("Unpacking DISPLAY_INPUT\n");
            done = parseDisplay(payloadParseIndex++, pHeader->packetSize, byteIn);
            break;
        }
        if (done)
        {
            // We are done parsing payload
            parsingMode = MAGICWORD;
            payloadParseIndex = 0;
            index = 0;
        }
        break;
    }
    index++;

    return tempBool;
}

bool DisplayParser::parseMode(const int index, const int packetSize, const uint8_t byteIn)
{
    static PacketMode modePkt;

    modePkt.mode = byteIn;

    Serial.printf("Mode received. Mode: %d\n", modePkt.mode);

    m_display.setMode((MyDisplay::Mode)modePkt.mode);

    return true;
}

bool DisplayParser::parseEffect(const int index, const int packetSize, const uint8_t byteIn)
{
    static PacketEffect effectPkt;

    switch (index)
    {
    case 0:
        effectPkt.effect = byteIn;
        break;
    case 1:
        Serial.printf("Effect received. Effect: %d\n", effectPkt.effect);
        m_display.setEffect((MyDisplay::Effect)effectPkt.effect, byteIn /* permanent */);
        return true;
        break;
    }
    
    return false;
}

bool DisplayParser::parseCommand(const int index, const int packetSize, const uint8_t byteIn)
{
    static PacketCommand commandPkt;

    switch (index)
    {
    case 0:
        commandPkt.command = byteIn;
        break;
    case 1:
        commandPkt.value = byteIn;
        break;
    case 2:
        commandPkt.second_value = byteIn;
        Serial.printf("Command received: %d, %d, %d\n", 
                      commandPkt.command, 
                      commandPkt.value, 
                      commandPkt.second_value);

        actuateCommand((Command)commandPkt.command, commandPkt.value, commandPkt.second_value);
        return true;
        break;
    }
    return false;
}

bool DisplayParser::parseDisplay(const int index, const int packetSize, const uint8_t byteIn)
{
    static PacketDisplayUpdate displayPkt;
    static uint8_t pixelArr[4];
    //static Display::Pixel pixel;
    static MyDisplay::Pixel* pPixel = (MyDisplay::Pixel*)pixelArr;

    //Serial.printf("DisplayPart Index: %d, pixels: %d\n", index, displayPkt.pixels.size());
    switch (index)
    {
    case 0:
        // Parsing first byte; TypeOfUpdate
        
        displayPkt.typeOfUpdate = byteIn;
        displayPkt.pixels.clear();
        displayPkt.numOfPixels = 0;
        //Serial.printf("Rinsing pixel content. Num of pixels: %d\n", displayPkt.pixels.size());
        break;
    case 1:
        //Serial.printf("Layer: %d\n", byteIn);
        displayPkt.layer = byteIn;
        break;
    case 2:
    case 3:
        // Parsing second and third byte; NumberOfPixelUpdates

        //Serial.printf("Prev numOfPixels: %d, index: %d\n", displayPkt.numOfPixels, index);
        displayPkt.numOfPixels = displayPkt.numOfPixels << 1 | byteIn;
        //Serial.printf("New  numOfPixels: %d\n", displayPkt.numOfPixels);
        break;
    default:
        // Parsing pixels (We end up here many times. Until we have all pixels from numOfPixels)
        //Serial.printf("Processing pixel...\n");
        uint8_t innerIndex = (index - 4) % 4;
        pixelArr[innerIndex] = byteIn;

        //Serial.printf("innerIndex: %d, byteIn: %d\n", innerIndex, byteIn);

        if (innerIndex == 3)
        {
            displayPkt.pixels.push_back(*pPixel);
            //Serial.printf("Pushing new pixel with i: %d, r: %d, g: %d, b: %d\n", pPixel->index, pPixel->r, pPixel->g, pPixel->b);
        }
        if (displayPkt.pixels.size() == displayPkt.numOfPixels)
        {
            //Serial.printf("Display received. Size %d\n", displayPkt.pixels.size());
            //Serial.printf("First pixel with index %d: %d, %d, %d\n", displayPkt.pixels[0].index, displayPkt.pixels[0].r, displayPkt.pixels[0].g, displayPkt.pixels[0].b);
            actuateDisplay((DisplayUpdate)displayPkt.typeOfUpdate, displayPkt.pixels, (MyDisplay::Layer)displayPkt.layer);
            return true;
        }
        break;
    }

    return false;
}

void DisplayParser::actuateCommand(Command command, const uint8_t value, const uint8_t value2)
{
    switch (command)
    {
    case CLEAR:
        m_display.clear();
        break;
    case BRIGHTNESS:
        m_display.setBrightness(value);
        break;
    case LAYER_BRIGHTNESS:
        m_display.setLayerBrightness(value, value2);
        break;
    case TEST:
        m_display.test();
        break;
    case RENEW_TIME:
        m_clock.update();
        break;
    case RESET_WIFI:
        Settings::Instance()->resetWiFi();
        break;
    case CONFIGURE_WIFI:
        Settings::Instance()->reconfigure();
        break;
    }
}

void DisplayParser::actuateDisplay(DisplayUpdate update, const MyDisplay::PixelVec& pixels, MyDisplay::Layer layer)
{
    // TODO: Scary cast to "update". Same is declared in both Display.h and mt.h
    m_display.setPixels(pixels, layer, (MyDisplay::Update)update);
}