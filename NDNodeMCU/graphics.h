#pragma once

#include "display.h"
#include "clock.h"
#include "fonts.h"

void plotNum(Display& display, uint8_t num, uint8_t xPos, uint8_t yPos, uint32_t color);

void plotColumn(Display& display, uint8_t column, uint8_t xPos, uint32_t color);

//! Uses colors from clock
//! Assumes a 8x32 display
void plotClock(const Clock& clock, Display& display);