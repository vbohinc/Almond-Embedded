#ifndef __COMMON__
#define __COMMON__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "error.h"
#include <util/delay.h>

// X = Port, Y = Bit

#define set_bit(X, Y) (X |= (1<<(Y)))
#define clear_bit(X, Y) (X &= ~(1<<(Y)))
#define check_bit(X, Y) ((X&(1<<Y)) != 0)

#define while_timeout(X, Y) for(uint16_t __timeout = 0; __timeout++ <= Y && (X); _delay_ms(Y ? 1 : 0))

#endif
