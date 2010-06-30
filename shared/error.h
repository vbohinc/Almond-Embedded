/*
* Error handling functions.
*/

#ifndef __ERROR__
#define __ERROR__

#include "common.h"
#include <avr/pgmspace.h>


void assert (bool condition, const char *msg);
void info (const char *msg);
void warn(const char *msg);
void error(const char *msg);
void debug(const char *msg);

void assert_pgm(bool condition, const prog_char *msg);
void info_pgm (const prog_char *msg);
void warn_pgm(const prog_char *msg);
void error_pgm(const prog_char *msg);
void debug_pgm(const prog_char *msg);

#endif
