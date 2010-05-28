/*
* Error handling functions.
*/

#ifndef __ERROR__
#define __ERROR__

#include "common.h"

void assert (bool condition, const char *msg);
void info (const char *msg);
void warn(const char *msg);
void error(const char *msg);
void debug(const char *msg);

#endif
