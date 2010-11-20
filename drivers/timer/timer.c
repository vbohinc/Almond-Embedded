/*     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 */

#include <avr/io.h>
#include "timer.h"

uint16_t timerstack[3];
uint8_t timerstack_count = 0;

void
timer_init ()
{
    RTC.CTRL = RTC_PRESCALER_DIV1_gc;
    CLK.RTCCTRL |= CLK_RTCEN_bm;
}

static void
timer_wait()
{
    while((RTC.STATUS & RTC_SYNCBUSY_bm) != 0);
}

void timer_set(uint16_t time)
{
    timer_wait();
    RTC.COMP = time;
    timer_wait();
    RTC.CNT = 0;
    RTC.INTFLAGS = 0x03;
}

void timer_reset(void )
{
    timer_wait();
    RTC.COMP = 0;
}

bool timer_reached(void )
{
    return ((RTC.INTFLAGS & 0x02) != 0);
}

uint16_t timer_remaining()
{
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
    if(timerstack_count < 4)
    {
        timer_set(time);
        if (timerstack_count != 0)
        {
            if(timer_remaining() > time)
                timerstack[timerstack_count-1] = timer_remaining()-time;
            else
                timerstack[timerstack_count-1] = 0;
        }
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
        else if (timerstack_count < 4)
        {
            if(timerstack[timerstack_count-1] != 0)
                timer_set(timer_remaining()+timerstack[timerstack_count-1]);
        }              
        return false;
    }
    else
        return true;
}
