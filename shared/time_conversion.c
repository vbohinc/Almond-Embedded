#include "time_conversion.h"

/* Copyright (C) 1991, 1993, 1997, 1998, 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#define SECS_PER_HOUR (60 * 60)
#define SECS_PER_DAY (SECS_PER_HOUR * 24)

/** Nonzero if YEAR is a leap year (every 4 years,
   except every 100th isn't, and every 400th is).  */
#define __isleap(year) \
    ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))



/* Compute the `struct tm' representation of *T,
   offset OFFSET seconds east of UTC,
   and store year, yday, mon, mday, wday, hour, min, sec into *TP.
   Return nonzero if successful.  */
bool timestamp_to_time (uint32_t timestamp, uint16_t offset, struct time_struct *tm)
{
    const unsigned short int __mon_yday[2][13] =
    {
        /* Normal years.  */
        { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
        /* Leap years.  */
        { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
    };

    long int days, rem, y;
    const unsigned short int *ip;

    days = timestamp / SECS_PER_DAY;
    rem = timestamp % SECS_PER_DAY;
    rem += offset;

    while (rem < 0)
    {
        rem += SECS_PER_DAY;
        --days;
    }

    while (rem >= SECS_PER_DAY)
    {
        rem -= SECS_PER_DAY;
        ++days;
    }

    tm->tm_hour = rem / SECS_PER_HOUR;

    rem %= SECS_PER_HOUR;
    tm->tm_min = rem / 60;
    tm->tm_sec = rem % 60;
    /* January 1, 1970 was a Thursday.  */
    tm->tm_wday = (4 + days) % 7;

    if (tm->tm_wday < 0)
        tm->tm_wday += 7;

    y = 1970;

#define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

    while (days < 0 || days >= (__isleap (y) ? 366 : 365))
    {
        /* Guess a corrected year, assuming 365 days per year.  */
        long int yg = y + days / 365 - (days % 365 < 0);

        /* Adjust DAYS and Y to match the guessed year.  */
        days -= ( (yg - y) * 365
                  + LEAPS_THRU_END_OF (yg - 1)
                  - LEAPS_THRU_END_OF (y - 1));
        y = yg;
    }

    tm->tm_year = y - 1900;

    if (tm->tm_year != y - 1900)
    {
        /* The year cannot be represented due to overflow.  */
        //__set_errno (EOVERFLOW);
        return false;
    }

    tm->tm_yday = days;

    ip = __mon_yday[__isleap (y) ];

    for (y = 11; days < (long int) ip[y]; --y)
        continue;

    days -= ip[y];

    tm->tm_mon = y;

    tm->tm_mday = days + 1;

    return true;
}

uint32_t time_to_timestamp (uint16_t year, uint8_t month, uint8_t day,
                            uint8_t hour, uint8_t minute, uint8_t second)
{
    const short tage_bis_monatsanfang[12] = /* ohne Schalttag */
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    long long unix_zeit;
    long long jahre = year - 1970;
    int schaltjahre = ( (year - 1) - 1968) / 4 - ( (year - 1) - 1900) / 100 + ( (year - 1) - 1600) / 400;

    unix_zeit = second + 60 * minute + 60 * 60 * hour +
                (tage_bis_monatsanfang[month-1] + day - 1) * 60 * 60 * 24 +
                (jahre * 365 + schaltjahre) * 60 * 60 * 24;

    if ( (month > 2) && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)))
        unix_zeit += 60 * 60 * 24; /* +Schalttag wenn jahr Schaltjahr ist */

    return unix_zeit;
}

uint32_t timestruct_to_timestamp (struct time_struct tm)
{
    return time_to_timestamp (tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_mon, tm.tm_sec);
}

