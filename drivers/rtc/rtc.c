/**
 * RTC / timer driver for a PCF 8535 on the TWI interface
 * @file rtc.c
 * @author Linus Lotz
 */

#include <avr/io.h>
#include <rtc/rtc.h>
#include <error.h>

uint16_t timeH

ISR(RTC_OVF_vect)
{
	timeH++;
}


void rtc_init()
{
	sei();
	RTC.CTRL = RTC_PRESCALER_DIV1024_gc;
	CLK.RTCCTRL = CLK_RTCSRC_RCOSC_gc | CLK_RTCEN_bm;
	RTC.INTCTRL = RTC_OVFINTLVL_LO_gc;
}

uint32_t get_time()
{
	return ((uint32_t)timeH)<<16 + RTC.CNT;
}

void set_time (uint32_t time)
{
	RTC.CNT = time;
	timeH = time>>16;
}

