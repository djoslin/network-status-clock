#ifndef TTF_FONTS_H
#define TTF_FONTS_H

#include <Arduino.h>

enum TTF_Font {
    NONE,
    ROBOTO_REGULAR,
    DSEG7
};

struct TTF_FontMetric {
    TTF_Font font;
    float scale;
};

const TTF_FontMetric ttfFontMetrics[] = {{ROBOTO_REGULAR, 1.37}};

extern const byte robotoRegular_start[] asm("_binary_fonts_RobotoRegular_ttf_start");
extern const byte robotoRegular_end[] asm("_binary_fonts_RobotoRegular_ttf_end");

extern const byte dseg7_start[] asm("_binary_fonts_DSEG7ModernBold_ttf_start");
extern const byte dseg7_end[] asm("_binary_fonts_DSEG7ModernBold_ttf_end");

#endif
