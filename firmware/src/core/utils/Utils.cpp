#include "Utils.h"

uint16_t Utils::rgb565dim(uint16_t color, uint8_t brightness, bool swapBytes) {
    if (color == 0 || brightness == 0) {
        return 0;
    }
    if (swapBytes) {
        // swap bytes
        color = (color >> 8) | (color << 8);
    }

    // Extract 5-bit Red, 6-bit Green, and 5-bit Blue components
    uint8_t r5 = (color >> 11) & 0x1F;
    uint8_t g6 = (color >> 5) & 0x3F;
    uint8_t b5 = color & 0x1F;

    // Scale brightness (0-255) to (0-32) and (0-64) ranges
    uint16_t r5_dim = (r5 * brightness + 127) / 255; // Round by adding 127
    uint16_t g6_dim = (g6 * brightness + 127) / 255;
    uint16_t b5_dim = (b5 * brightness + 127) / 255;

    // Recombine into RGB565 format
    uint16_t result = (r5_dim << 11) | (g6_dim << 5) | b5_dim;

    if (swapBytes) {
        // swap bytes
        result = (result >> 8) | (result << 8);
    }
    return result;
}
