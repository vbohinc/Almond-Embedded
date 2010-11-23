/*     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 */
/**
 * \file timer.c
 * \brief A timer driver for XMega
 * 
 * This is a timer driver for XMega, to properly implement timeouts
 */

#include <common.h>

extern void timer_init(void);

extern void timer_set(uint16_t time);

extern void timer_reset(void);

extern bool timer_reached(void);


#define while_timeout(X, Y) for(__timer_set_helper(Y); __timer_timeout_helper(X); )

extern void __timer_set_helper(uint16_t time);

extern bool __timer_timeout_helper(bool cond);
