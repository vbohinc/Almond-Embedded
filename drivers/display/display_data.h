#include <avr/pgmspace.h>
#include "../../shared/common.h"
//#include "display_data.h"

#ifndef __DISPLAY_DATA_H__
#define __DISPLAY_DATA_H__

#define FONT_CHAR_MIN 2
#define FONT_CHAR_MAX 95
#define FONT_CHAR_ASCII_OFFSET 32

// FONT SIZE: 0 (6x8)
PROGMEM uint8_t font_0[96];

// FONT SIZE: 1 (7x13)
PROGMEM uint8_t font_1[96];

// FONT SIZE: 2 (9x15)
PROGMEM uint8_t font_2[96];

#endif
