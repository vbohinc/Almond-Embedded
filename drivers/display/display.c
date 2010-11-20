/**
 * the display driver
 * @file display.h
 * @addtogroup display
 * @author Thomas Parsch
*/

#include "display.h"

bool inverted = false;

void
display_set_inverted (bool value)
{
    inverted = value;
}

bool
display_get_inverted (void)
{
    return inverted;
}

#ifdef X86
#include "display_x86.c"
#else
#include "display_avr.c"
#endif
