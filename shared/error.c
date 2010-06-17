 /**
 * Error handling functions
 */

#include <stdbool.h>
#include "ftdi.h"

char error_builder[255];
 
void assert (bool condition, const char *msg) {
  if (condition) {
#ifdef SERIAL
    exit(1);
#else
    FTDISend('A');
    FTDISend('S');
    FTDISend('S');
    FTDISend(':');
    uint8_t i = 0;
    while (msg[i]!=0 && i<255)
        FTDISend(msg[i]);
    FTDISend('\n');
#endif
  }
}

void info (const char *msg)
{
#ifdef SERIAL
	printf("[INFO]: %s\n", msg);
#else
    FTDISend('I');
    FTDISend('N');
    FTDISend('F');
    FTDISend(':');
    for (uint8_t i=0; i<255 && msg[i]!='\0'; i++)
        FTDISend(msg[i]);
    FTDISend('\n');
#endif
	return;
}

void warn(const char *msg)
{
#ifdef SERIAL
	printf("[WARN]: %s\n", msg);
#else
    FTDISend('W');
    FTDISend('R');
    FTDISend('N');
    FTDISend(':');
    for (uint8_t i=0; i<255 && msg[i]!='\0'; i++)
        FTDISend(msg[i]);
    FTDISend('\n');
#endif
	return;
}

void error(const char *msg)
{
#ifdef SERIAL
	printf("[ERROR]: %s\n", msg);
#else
    FTDISend('E');
    FTDISend('R');
    FTDISend('R');
    FTDISend(':');
    for (uint8_t i=0; i<255 && msg[i]!='\0'; i++)
        FTDISend(msg[i]);
    FTDISend('\n');
#endif
	return;
}

void debug(const char *msg)
{
#ifdef SERIAL
	printf("[DEBUG]: %s\n", msg);
#else
    FTDISend('D');
    FTDISend('B');
    FTDISend('G');
    FTDISend(':');
    for (uint8_t i=0; i<255 && msg[i]!='\0'; i++)
        FTDISend(msg[i]);
    FTDISend('\n');
#endif
	return;
}
