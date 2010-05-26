#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <avr/io.h>
#include <avr/interrupt.h>

void start_sleep(uint16_t sec);
void init_timer(void);

#endif // TIMER_H_INCLUDED

