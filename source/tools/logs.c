#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>

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
    } else if (mode == NORMAL) {
      FILE *file = fopen("logs", "a");
      char ftext[256], date[128];

      time_t t = time(NULL);
      struct tm *tm = localtime(&t);
      sprintf(date, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

      if (type == INFO) {
        sprintf(ftext, "[%s] [++]: %s\n", date, formatted);
        printf("[\033[0;34m++\033[0m]: %s\n", formatted);
        fputs(ftext, file);
        fclose(file);
      } else if (type == WARN) {
        sprintf(ftext, "[%s] [WW]: %s\n", date, formatted);
        printf("[\033[0;33mWW\033[0m]: %s\n", formatted);
        fputs(ftext, file);
        fclose(file);
      } else if (type == ERROR) {
        sprintf(ftext, "[%s] [EE]: %s\n", date, formatted);
        printf("[\033[0;31mEE\033[0m]: %s\n", formatted);
        fputs(ftext, file);
        fclose(file);
        exit(EXIT_FAILURE);
      }
    }
  }
#endif
