/**
 * Driver for the Buttons on XMega
 * @file button.c
 * @author Stefan Profanter
 */
#include "buttons.h"
#include <gui/gui.h>


#ifdef X86
#include "buttons_x86.c"
#else
#include "buttons_avr.c"
#endif

