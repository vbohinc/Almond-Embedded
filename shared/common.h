/**
 * header for commmon
 * common includes and code parts
 * @file common.h
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

/// Set on Port X Bit Y
#define set_bit(X, Y) (X |= (1<<(Y)))
/// Clear on Port X Bit Y
#define clear_bit(X, Y) (X &= ~(1<<(Y)))
/// Check on Port X if Bit y is set
#define check_bit(X, Y) ((X&(1<<Y)) != 0)

#if __AVR_ARCH__ >= 100
#include <timer/timer.h>
#else
//TODO X Conditon
/// Set a timeout of Y ms and a Conditon X, which have to be true while timeout
#define while_timeout(X, Y) for(uint16_t __timeout = 0; __timeout++ <= Y && (X); _delay_ms(Y ? 1 : 0))
#endif

#endif
