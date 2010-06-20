/*
* Error handling functions.
*/

#ifndef __ERROR__
#define __ERROR__

#include "common.h"
//#include <stdio.h>
#include "string_pool.h"


extern char error_builder[100];

void assert (bool condition, const char *msg);
void info (const char *msg);
void warn(const char *msg);
void error(const char *msg);
void debug(const char *msg);

void send_eeprom(uint8_t *msg);

void assert_eeprom (bool condition, const uint8_t *msg);
void info_eeprom (const uint8_t *msg);
void warn_eeprom(const uint8_t *msg);
void error_eeprom(const uint8_t *msg);
void debug_eeprom(const uint8_t *msg);

#endif
