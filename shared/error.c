 /**
 * Error handling functions
 */

#include <stdbool.h>
 
void assert (bool condition, const char *msg) {
  if (condition) {
#ifdef SERIAL
    exit(1);
#endif
  }
}

void info (const char *msg)
{
#ifdef SERIAL
	printf("[INFO]: %s\n", msg);
#endif
	return;
}

void warn (const char *msg)
{
#ifdef SERIAL
	printf("[WARN]: %s\n", msg);
#endif
	return;
}

void error (const char *msg)
{
#ifdef SERIAL
	printf("[ERROR]: %s\n", msg);
#endif
	return;
}

void debug (const char *msg)
{
#ifdef SERIAL
	printf("[DEBUG]: %s\n", msg);
#endif
	return;
}
