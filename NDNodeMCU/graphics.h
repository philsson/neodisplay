#pragma once

#include "display.h"
#include "clock.h"
#include "fonts.h"

void plotNum(Display& display, uint8_t num, uint8_t xPos, uint8_t yPos, uint32_t color);

void plotColumn(Display& display, uint8_t column, uint8_t xPos, uint32_t color);

//! Uses colors from clock
//! Assumes a 8x32 display
void plotClock(const Clock& clock, Display& display);

void plotArr(Display& display,
             const byte arr[],
             uint8_t width, 
             uint8_t height,
             uint8_t xPos, 
             uint8_t yPos, 
             uint32_t color,
             Display::Layer layer = Display::Layer::MIDLAYER);

//! Animation of WiFi Icon which progresses on each plot
//! @doneConnecting: if true a symbol will show with the connection result
//! @connected: "the result". Shows a green tick if true, red "x" if false
//! Return: true when it has pulsed one time
bool plotWiFi(Display& display, bool doneConnecting = false, bool connected = false);