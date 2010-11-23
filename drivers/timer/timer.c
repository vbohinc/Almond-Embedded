/*     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 */

#include <avr/io.h>
#include "timer.h"

#define TIMER_STACK_SIZE 5

uint16_t timerstack[TIMER_STACK_SIZE];
uint8_t timerstack_count = 0;

void
timer_init ()
{
    CLK.RTCCTRL |= CLK_RTCEN_bm;
}

static void
timer_wait(void)
{
    while((RTC.STATUS & RTC_SYNCBUSY_bm) != 0);
}

void timer_set(uint16_t time)
{
    timer_wait();
    RTC.COMP = (time+3+RTC.CNT)%65536;
    if((RTC.INTFLAGS & 0x02) != 0)
		RTC.INTFLAGS = 0x02;
    RTC.CTRL = RTC_PRESCALER_DIV2_gc;
}

void timer_reset(void )
{
    timer_wait();
	RTC.CTRL = 0;
    RTC.COMP = 0;
	RTC.CNT = 0;
    if((RTC.INTFLAGS & 0x02) != 0)
		RTC.INTFLAGS = 0x02;
}

bool timer_reached(void )
{
	timer_wait();
    return ((RTC.INTFLAGS & 0x02) != 0);
}

uint16_t timer_remaining(void)
{
	timer_wait();
    return (RTC.COMP>RTC.CNT) ? RTC.COMP-RTC.CNT : 0;
}

void __timer_set_helper(uint16_t time)
{
    if(time == 0)
        return;
    if(timer_reached())
        return;
    //won't harm to be sure
    timer_init();
    if(timerstack_count <= TIMER_STACK_SIZE)
    {
        if (timerstack_count != 0)
        {
            if(timer_remaining() > time)
            {
                timerstack[timerstack_count-1] = timer_remaining()-time;
                timer_set(time);
            }
            else
                timerstack[timerstack_count-1] = 0;
        }
        else
            timer_set(time);
    }
    timerstack_count++;
}


bool __timer_timeout_helper(bool cond)
{
    if(timer_reached() || !cond)
    {
        timerstack_count--;
        if(timerstack_count == 0)
            timer_reset();
        else if (timerstack_count <= TIMER_STACK_SIZE)
        {
            if(timerstack[timerstack_count-1] != 0)
                timer_set(timer_remaining()+timerstack[timerstack_count-1]);
        }
        return false;
    }
    else
        return true;
}

