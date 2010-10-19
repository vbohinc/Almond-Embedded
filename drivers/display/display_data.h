#ifndef X86
#include <avr/pgmspace.h>
#endif

#include "../../shared/common.h"

#ifndef __DISPLAY_DATA_H__
#define __DISPLAY_DATA_H__

#define FONT_CHAR_MIN 1
#define FONT_CHAR_MAX 95
#define FONT_CHAR_ASCII_OFFSET 31

// FONT SIZE: 0 (6x8)
extern const uint8_t font_0[][6];

// FONT SIZE: 1 (7x13)
extern const uint8_t font_1[][12];

// FONT SIZE: 2 (9x15)
extern const uint8_t font_2[][17];

// Almond logo
extern const uint8_t image_logo[];

extern const uint8_t testimg[];


extern const uint8_t tum_logo_f1[];
extern const uint8_t tum_logo_f2[];
extern const uint8_t tum_logo_f3[];
extern const uint8_t tum_logo_f4[];
extern const uint8_t tum_logo_f5[];
extern const uint8_t tum_logo_f6[];
extern const uint8_t tum_logo_f7[];
extern const uint8_t tum_logo_f8[];

#endif
