/**
 * \brief A timer driver for XMega
 * This is a timer driver for XMega, to properly implement timeouts
 * @file timer.h
 */

#include <common.h>

///Initialize the timer

/**
* Set the timer to a time
* @param time the timer is set to
*/
extern void timer_init(void);

///Reset the timer
extern void timer_set(uint16_t time);

/**
* Wait till timer is reached
* @return TODO
*/
extern void timer_reset(void);

extern bool timer_reached(void);


#define while_timeout(X, Y) for(__timer_set_helper(Y); __timer_timeout_helper(X); )

extern void __timer_set_helper(uint16_t time);

extern bool __timer_timeout_helper(bool cond);
