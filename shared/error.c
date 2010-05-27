/**
 * Error handling functions
 */

#include <stdbool.h>
 
void assert (bool condition, const char *msg) {
  if (condition) {
    exit(1);
  }
}
