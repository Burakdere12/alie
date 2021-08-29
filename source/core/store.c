#include <jansson.h>

#ifndef STORE
  #define STORE 1

  struct Store {
    json_t *user;
  } store;
#endif
