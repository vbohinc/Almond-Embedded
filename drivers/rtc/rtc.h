/**
 RTC / timer driver for PCF 8535
 */

#ifndef __RTC_H_
#define __RTC_H_

#include "common.h"

struct __time_struct {
  uint8_t  sec;   // Seconds      [0-59] (no leap second!)
  uint8_t  min;   // Minutes      [0-59]
  uint8_t  hour;  // Hour         [0-23]
  uint8_t  mday;  // Day in Month [1-31] 
  uint8_t  month; // Month        [1-12]
  uint16_t year;  // Year - 1900
  uint8_t  wday;  // Day of Week  [0-6]
};

typedef struct __time_struct  time_t;

time_t get_time(void);

void set_time(time_t*);

#endif //__RTC_H_