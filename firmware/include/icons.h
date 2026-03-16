#ifndef ICONS_H
#define ICONS_H

#include <Arduino.h>

// These symbols are generated from the files specified in platformio.ini under 'board_build.embed_files'
// See https://docs.platformio.org/en/latest/platforms/espressif32.html#embedding-binary-data for more info

extern const byte logo_start[] asm("_binary_images_logo_jpg_start");
extern const byte logo_end[] asm("_binary_images_logo_jpg_end");

#endif
