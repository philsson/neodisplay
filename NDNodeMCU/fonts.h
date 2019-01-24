#pragma once

#include <Arduino.h>
#include <vector>

#define FONT_WIDTH 4
#define FONT_HEIGHT 8


namespace fonts {

//! Definition of a font for numbers
typedef struct { char c; byte d[FONT_HEIGHT]; } Digits;

namespace std {

extern const Digits digits[10];

}
}