/**
 * RTC / timer driver for a PCF 8535 on the TWI interface
 * @file rtc.h
 * @author Linus Lotz
 */
#ifndef __RTC_H_
#define __RTC_H_

#include "common.h"

///rtc time struct
struct __time_struct
{
    uint8_t  sec;   ///< Seconds      [0-59] (no leap second!)
    uint8_t  min;   ///< Minutes      [0-59]
    uint8_t  hour;  ///< Hour         [0-23]
    uint8_t  mday;  ///< Day in Month [1-31]
    uint8_t  month; ///< Month        [1-12]
    uint16_t year;  ///< Year - 1900
    uint8_t  wday;  ///< Day of Week  [0-6]
};

typedef struct __time_struct  time_t;

/**
*Gets the time
* @return time_t structure with the current time
*/
time_t get_time (void);

/**
* Sets the time
* @param time pointer to time_t struct to set the time
*/
void set_time (time_t* time);

#endif //__RTC_H_
