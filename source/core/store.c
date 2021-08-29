#include <jansson.h>

#ifndef STORE
  #define STORE 1

  struct Store {
    json_t *user;
    char *token;
    size_t guild_count;
  } store;
#endif
