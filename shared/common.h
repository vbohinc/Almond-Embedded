/**
 * Common headerfiles
 */
 
#ifndef __COMMON__
#define __COMMON__

#include <stdint.h>
#include <stdbool.h>
#include "error.h"

// X = Port, Y = Bit

#define set_bit(X, Y) (X |= (1<<(Y)))
#define clear_bit(X, Y) (X &= ~(1<<(Y)))
#define check_bit(X, Y) ((X&(1<<Y)) != 0)

#endif
