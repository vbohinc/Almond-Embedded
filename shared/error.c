 /**
 * Error handling functions
 */

#include <stdbool.h>
#include "ftdi.h"

#include <avr/pgmspace.h>

inline void _send_msg(const char *msg)
{
  for (uint8_t i=0; i<255 && msg[i]!='\0'; i++)
  {
    FTDISend(msg[i]);
  }
  FTDISend('\n');
}

void assert (bool condition, const char *msg) {
  if (condition) {
#ifdef SERIAL
    exit(1);
#else
    _send_msg("ASS:");
    _send_msg(msg);
#endif
  }
}

void info (const char *msg)
{
#ifdef SERIAL
	printf("[INFO]: %s\n", msg);
#else
    _send_msg("INF:");
    _send_msg(msg);
#endif
}

void warn (const char *msg)
{
#ifdef SERIAL
	printf("[WARN]: %s\n", msg);
#else
    _send_msg("WARN:");
    _send_msg(msg);
#endif
}

void error (const char *msg)
{
#ifdef SERIAL
	printf("[ERROR]: %s\n", msg);
#else
    _send_msg("ERR:");
    _send_msg(msg);
#endif
}

void debug (const char *msg)
{
#ifdef SERIAL
	printf("[DEBUG]: %s\n", msg);
#else
    _send_msg("DBG:");
    _send_msg(msg);
#endif
}

void send_pgm(const prog_char *msg)
{
	uint8_t  myByte;
        myByte = pgm_read_byte(msg);
	for(int i = 1; myByte != '\0'; i++)
	{
		 FTDISend(myByte);
                 myByte = pgm_read_byte(msg+i);
	}
}


void assert_pgm(bool condition, const prog_char *msg) {
	if (condition) {
		send_pgm(PSTR("ASS:"));
		send_pgm(msg);
		FTDISend('\n');
	}
}

void info_pgm(const prog_char *msg)
{
	send_pgm(PSTR("INF:"));
	send_pgm(msg);
	FTDISend('\n');
}

void warn_pgm(const prog_char *msg)
{
	send_pgm(PSTR("WARN:"));
	send_pgm(msg);
	FTDISend('\n');
}

void error_pgm(const prog_char *msg)
{
	send_pgm(PSTR("ERR:"));
	send_pgm(msg);
	FTDISend('\n');
}

void debug_pgm(const prog_char *msg)
{
	send_pgm(PSTR("DBG:"));
	send_pgm(msg);
	FTDISend('\n');
}

