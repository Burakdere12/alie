#include <stdio.h>
#include <jansson.h>

#include "../core/request.c"
#include "../core/store.c"

#include "../tools/logs.c"

void READY() {
  logs(DEBUG, INFO, "This project was compiled at %s on %s", __TIME__, __DATE__);
  logs(NORMAL, INFO, "%s#%s is started!", json_string_value(json_object_get(store.user, "username")), json_string_value(json_object_get(store.user, "discriminator")));
}
