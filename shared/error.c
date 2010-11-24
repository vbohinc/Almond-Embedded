/**
 * Error output Driver
 * @file error.c
 */

#include <stdbool.h>
#include "ftdi.h"
#include <display/display.h>

#include <avr/pgmspace.h>
#include "error_driver.h"

inline void _send_msg (const char *msg)
{
    for (uint8_t i = 0; i < 255 && msg[i] != '\0'; i++)
    {
        error_driver_write_c (msg[i]);
    }

    error_driver_write_c ('\n');
}

void send_pgm (const prog_char *msg)
{
    uint8_t  myByte;
    myByte = pgm_read_byte (msg);

    for (int i = 1; myByte != '\0'; i++)
    {
        error_driver_write_c (myByte);
        myByte = pgm_read_byte (msg + i);
    }

#ifdef DEBUG_TO_DISPLAY
    display_flip();

#endif
}

uint8_t char_to_hex (uint8_t chr)
{
    //convert char number to int
    if (chr >= '0' && chr <= '9')
        return chr -'0';
    else
        if (chr >= 'A' && chr <= 'F')
            return chr -'A' + 10;
        else
            if (chr >= 'a' && chr <= 'f')
                return chr -'a' + 10;
            else
                return 255;
}

uint8_t hex_to_char (uint8_t hex)
{
    if (hex <= 9)
        return hex + '0';
    else
        if (hex > 9 && hex <= 15)
            return hex + 'A' -10;
        else
            return 0;
}


#ifdef DEBUG

void error_init (void)
{
    error_driver_init();
}


void error_putc (const char c)
{
    error_driver_write_c (c);
#ifdef DEBUG_TO_DISPLAY
    display_flip();
#endif
}


void assert (bool condition, const char *msg)
{
    if (!condition)
    {
        send_pgm (PSTR ("ASS:"));
        _send_msg (msg);
    }
}

void info (const char *msg)
{
    send_pgm (PSTR ("INF:"));
    _send_msg (msg);
}

void warn (const char *msg)
{
    send_pgm (PSTR ("WARN:"));
    _send_msg (msg);
}

void debug (const char *msg)
{
    send_pgm (PSTR ("DBG:"));
    _send_msg (msg);
}

void error (const char *msg)
{
    send_pgm (PSTR ("ERR:"));
    _send_msg (msg);
}

#endif

#ifdef DEBUG
void assert_pgm (bool condition, const prog_char *msg)
{
    if (condition)
    {
        send_pgm (PSTR ("ASS:"));
        send_pgm (msg);
        error_driver_write_c ('\n');
    }
}

void info_pgm (const prog_char *msg)
{
    send_pgm (PSTR ("INF:"));
    send_pgm (msg);
    error_driver_write_c ('\n');
}

void warn_pgm (const prog_char *msg)
{
    send_pgm (PSTR ("WARN:"));
    send_pgm (msg);
    error_driver_write_c ('\n');
}


void error_pgm (const prog_char *msg)
{
    send_pgm (PSTR ("ERR:"));
    send_pgm (msg);
    error_driver_write_c ('\n');
}

void debug_pgm (const prog_char *msg)
{
    send_pgm (PSTR ("DBG:"));
    send_pgm (msg);
    error_driver_write_c ('\n');
}

void print_hex (uint8_t num)
{
    uint8_t chr = hex_to_char (num);

    if (chr == 0)
        error_putc ('#');
    else
        error_putc (chr);
}

void byte_to_hex (uint8_t byte)
{
    uint8_t b2 = (byte & 0x0F);
    uint8_t b1 = ( (byte & 0xF0) >> 4);
    print_hex (b1);
    print_hex (b2);
#ifdef DEBUG_TO_DISPLAY
    display_flip();
#endif
}

#endif

