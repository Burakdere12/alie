#include <string.h>
#include <stdbool.h>

#ifndef CONFIGURATION
  #define CONFIGURATION 1

  struct Configuration {
    char* token;
    char* developers[2];
    char* prefix;
    bool debug;
  } config;

  void init_configuration() {
    config.token = "";
    config.developers[0] = "";
    config.developers[1] = "";
    config.prefix = "";
    config.debug = false;
  }
#endif
