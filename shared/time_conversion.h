#ifndef __TIME_CONVERSION__
#define __TIME_CONVERSION__

#include <stdint.h>
#include <stdbool.h>

/**
 * contains the data for time comversion from and to unix timestamp
 * uint8_t    tm_sec   seconds [0,61]
 * uint8_t    tm_min   minutes [0,59]
 * uint8_t    tm_hour  hour [0,23]
 * uint8_t    tm_mday  day of month [1,31]
 * uint8_t    tm_mon   month of year [0,11]
 * uint16_t   tm_year  years since 1900
 * uint8_t    tm_wday  day of week [0,6] (Sunday = 0)
 * uint16_t   tm_yday  day of year [0,365]
 */

struct time_struct
{
    uint8_t    tm_sec;
    uint8_t    tm_min;
    uint8_t    tm_hour;
    uint8_t    tm_mday;
    uint8_t    tm_mon;
    uint16_t   tm_year;
    uint8_t    tm_wday;
    uint16_t   tm_yday;
};

/**
 * Comverts a unix timestamp into the time_struct format.
 * @param timestamp the unix timestamp
 * @param offset OFFSET seconds east of UTC
 * @param tm time_struct to store converted unix timestamp into
 */
extern bool timestamp_to_time (uint32_t timestamp, uint16_t offset, struct time_struct *tm);

/**
 * Converts the content of time_struct into a unix timestamp.
 * time_struct must be cleaned off time zones. Must be UTC.
 * @param tm time_struct to convert (uses only tm_year, tm_mon, tm_mday, tm_hour, tm_min, tm_sec)
 * @return the unix timestamp
 */
extern uint32_t timestruct_to_timestamp (struct time_struct tm);

/**
 * Converts the time with given parameters into a unix timestamp.
 * time must be in UTC.
 * @param year
 * @param month
 * @param day
 * @param hour
 * @param minute
 * @param second
 * @return the unix timestamp
 */
extern uint32_t time_to_timestamp (uint16_t year, uint8_t month, uint8_t day,
                                       uint8_t hour, uint8_t minute, uint8_t second);


#endif //__TIME_CONVERSION__
