/**
 * RTC / timer driver for a PCF 8535 on the TWI interface
 * @file rtc.h
 * @author Linus Lotz
 */
#ifndef __RTC_H_
#define __RTC_H_

#include "common.h"

void rtc_init(void);

uint32_t get_time (void);

void set_time (uint32_t);

#endif //__RTC_H_
