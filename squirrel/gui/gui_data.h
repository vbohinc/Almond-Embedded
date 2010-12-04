/**
 * GUI Data
 * @file gui_data.h
 * @author Matthias Schwab
 */
#ifndef X86
#include <avr/pgmspace.h>
#endif

#include <common.h>

#ifndef __DISPLAY_DATA_H__
#define __DISPLAY_DATA_H__

#define FONT_CHAR_MIN 1
#define FONT_CHAR_MAX 95
#define FONT_CHAR_ASCII_OFFSET 31

/// data for font with size: 0 (6x8)
extern const uint8_t font_0[][6];

/// data for font with size: 1 (7x13)
extern const uint8_t font_1[][12];

/// data for font with size: 2 (9x15)
extern const uint8_t font_2[][17];

/// Almond logo data
extern const uint8_t image_logo[];

/// Unknown test image
extern const uint8_t testimg[];

/// Left arrow for button bar
extern const uint8_t arrow_left[];
/// Right arrow for button bar
extern const uint8_t arrow_right[];
/// Up arrow for button bar
extern const uint8_t arrow_up[];
/// Down arrow for button bar
extern const uint8_t arrow_down[];

/// Frame 1 for animated almonf logo
extern const uint8_t almond_logo_f1[];
/// Frame 2 for animated almonf logo
extern const uint8_t almond_logo_f2[];
/// Frame 3 for animated almonf logo
extern const uint8_t almond_logo_f3[];
/// Frame 4 for animated almonf logo
extern const uint8_t almond_logo_f4[];

/// Matthias Credit portrait
extern const uint8_t team_matthias[];
/// Sean Credit portrait
extern const uint8_t team_sean[];
/// Stefan Credit portrait
extern const uint8_t team_stefan[];
/// Linus Credit portrait
extern const uint8_t team_linus[];
/// Pascal Credit portrait
extern const uint8_t team_pascal[];
/// Salomon Credit portrait
extern const uint8_t team_salomon[];
/// Thomas Credit portrait
extern const uint8_t team_thomas[];
/// Christian Credit portrait
extern const uint8_t team_christian[];
/// Maximilian Credit portrait
extern const uint8_t team_maximilian[];

extern const uint8_t bluetooth[];

#endif
