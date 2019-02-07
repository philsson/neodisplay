#pragma once

#include <Arduino.h>
#include <vector>

#define FONT_WIDTH 4
#define FONT_HEIGHT 8


namespace fonts {

//! Definition of a font for numbers
typedef struct { char c; byte d[FONT_HEIGHT]; } Font;

namespace std {

extern const Font digits[10];

} // namespace std

namespace drawings {

extern const Font wifi[4];

} // namespace drawings

} // namespace fonts