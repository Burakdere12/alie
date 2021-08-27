#include <stdio.h>
#include <jansson.h>

void READY(json_t *data) {
  json_t *user = json_object_get(data, "user");
  printf("%s#%s is started!\n", json_string_value(json_object_get(user, "username")), json_string_value(json_object_get(user, "discriminator")));
}
