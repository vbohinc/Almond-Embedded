/**
 * display.c - the display driver
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 * \author Thomas Parsch
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
