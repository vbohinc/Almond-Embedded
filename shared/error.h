/*
* Error handling functions.
*/

#ifndef __ERROR__
#define __ERROR__

#include "common.h"
//#include <stdio.h>

extern char error_builder[100];

void assert (bool condition, const char *msg);
void info (const char *msg);
void warn(const char *msg);
void error(const char *msg);
void debug(const char *msg);

#endif
