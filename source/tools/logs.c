#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include "../../configuration.c"

#ifndef LOG
  #define LOG 1

  #define DEBUG 0
  #define NORMAL 1

  #define INFO 0
  #define WARN 1
  #define ERROR 2

  void logs(const short mode, const short type, const char *text, ...) {
    char formatted[256];
    va_list args;
    va_start(args, text);
    vsprintf(formatted, text, args);
    va_end(args);

    if (mode == DEBUG && config.debug == true) {
      if (type == INFO) {
        printf("[\033[0;33mDD\033[0m] [\033[0;34m++\033[0m]: %s\n", formatted);
      } else if (type == WARN) {
        printf("[\033[0;33mDD\033[0m] [\033[0;33mWW\033[0m]: %s\n", formatted);
      } else if (type == ERROR) {
        printf("[\033[0;33mDD\033[0m] [\033[0;31mEE\033[0m]: %s\n", formatted);
      }
    }
  }
#endif
