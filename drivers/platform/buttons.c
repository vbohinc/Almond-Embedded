/**
 * button.c - Driver for the Buttons on XMega
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 * \author Stefan Profanter
 */
#include "buttons.h"
#include <gui/gui.h>


#ifdef X86
#include "buttons_x86.c"
#elif BUTTONS_REMOTE
#include "buttons_remote.c"
#else
#include "buttons_avr.c"
#endif

