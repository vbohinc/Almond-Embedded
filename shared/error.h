/*
* Error handling functions.
*/

#ifndef __ERROR__
#define __ERROR__

#include "common.h"
//#include <stdio.h>
#include "string_pool.h"


void assert (bool condition, const char *msg);
void info (const char *msg);
void warn(const char *msg);
void error(const char *msg);
void debug(const char *msg);

void send_progmem(uint8_t *msg);

void assert_progmem (bool condition, const prog_char *msg);
void info_progmem (const prog_char *msg);
void warn_progmem(const prog_char *msg);
void error_progmem(const prog_char *msg);
void debug_progmem(const prog_char *msg);

#endif
