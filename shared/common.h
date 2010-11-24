/**
 * common.h - Common stuff for the ALmond Project
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 */

#ifndef __COMMON__
#define __COMMON__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "error.h"
#ifndef X86
#include <util/delay.h>
#endif

// X = Port, Y = Bit


#define set_bit(X, Y) (X |= (1<<(Y)))
#define clear_bit(X, Y) (X &= ~(1<<(Y)))
#define check_bit(X, Y) ((X&(1<<Y)) != 0)

//#if __AVR_ARCH__ >= 100
//#include <timer/timer.h>
//#else
#define while_timeout(X, Y) for(uint16_t __timeout = 0; __timeout++ <= Y && (X); _delay_ms(Y ? 1 : 0))
//#endif

#endif
