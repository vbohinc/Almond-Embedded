/*
 * Error handling functions.
 */

#ifndef __ERROR__
#define __ERROR__

#include "common.h"
#include <avr/pgmspace.h>



#ifdef DEBUG

void error_init(void);

void error_putc(const char c);

void assert (bool condition, const char *msg);
void info (const char *msg);
void warn(const char *msg);
void debug(const char *msg);
void error(const char *msg);

void assert_pgm(bool condition, const prog_char *msg);
void info_pgm (const prog_char *msg);
void warn_pgm(const prog_char *msg);
void debug_pgm(const prog_char *msg);
void error_pgm(const prog_char *msg);

void byte_to_hex(uint8_t byte);

#else

#define error_init() {}


#define error_putc(c) {}

#define assert(cond, msg) {}
#define info(msg) {}
#define warn(msg) {}
#define debug(msg) {}
#define error(msg) {}

#define assert_pgm(cond, msg) {}
#define info_pgm(msg) {}
#define warn_pgm(msg) {}
#define debug_pgm(msg) {}
#define error_pgm(msg) {}
#define byte_to_hex(byte) {}
#endif
#endif

/**
 * Converts a char value 0-9,A-F,a-f to its corresponding int value
 * @param chr the char value
 * @return the int value
 */
extern uint8_t char_to_hex(uint8_t chr);

/**
 * Converts a hex value to its corresponding char value 0-9,A-F
 * @param hex the hex value
 * @return the char value
 */
extern uint8_t hex_to_char(uint8_t hex);
