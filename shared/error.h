/*
* Error handling functions.
*/

#ifndef __ERROR__
#define __ERROR__

#include "common.h"
#include <avr/pgmspace.h>


void error(const char *msg);

#ifdef DEBUG
void assert (bool condition, const char *msg);
void info (const char *msg);
void warn(const char *msg);
void debug(const char *msg);
#else
#define assert(cond, msg) {}
#define info(msg) {}
#define warn(msg) {}
#define debug(msg) {}
#endif

void error_pgm(const prog_char *msg);

#ifdef DEBUG 
void assert_pgm (bool condition, const prog_char *msg);
void info_pgm (const prog_char *msg);
void warn_pgm(const prog_char *msg);
void debug_pgm(const prog_char *msg);
#else
#define assert_pgm(cond, msg) {}
#define info_pgm(msg) {}
#define warn_pgm(msg) {}
#define debug_pgm(msg) {}
#endif

#endif
