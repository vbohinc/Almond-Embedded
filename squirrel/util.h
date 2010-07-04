/*
 *
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include "../shared/common.h"

bool time_set (uint32_t time);
uint32_t time_get (void);

bool log_write (uint8_t* mac, uint8_t id, uint32_t time, uint16_t value);
bool log_read (uint8_t* mac, uint8_t id, uint8_t page, uint8_t* buffer);

#endif
