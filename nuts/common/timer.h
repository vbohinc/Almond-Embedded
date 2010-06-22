#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <avr/io.h>
#include <avr/interrupt.h>

extern void start_sleep(uint16_t sec);
extern void init_timer(void);

#endif // TIMER_H_INCLUDED

