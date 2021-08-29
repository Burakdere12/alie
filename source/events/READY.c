#include <stdio.h>
#include <jansson.h>

#include "../core/store.c"

void READY() {
  printf("%s#%s is started!\n", json_string_value(json_object_get(store.user, "username")), json_string_value(json_object_get(store.user, "discriminator")));
}
