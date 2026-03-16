#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

#define ScreenWidth 240
#define ScreenHeight 240
#define ScreenCenterX (ScreenWidth / 2)
#define ScreenCenterY (ScreenHeight / 2)

class Utils {
public:
    static uint16_t rgb565dim(uint16_t color, uint8_t brightness, bool swapBytes = false);
};

#endif
