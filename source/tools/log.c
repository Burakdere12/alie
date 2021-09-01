#include <stdbool.h>
#include "configuration.c"

#ifndef LOG
  #define LOG 1

  #define DEBUG 0
  #define NORMAL 1

  // TODO: Write log method
  void log(short mode, char *text) {}
#endif
