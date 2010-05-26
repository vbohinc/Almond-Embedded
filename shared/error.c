/**
 * Error handling functions
 */
 
void assert (bool condition, const char *msg) {
  if (condition) {
    exit(1);
  }
}
