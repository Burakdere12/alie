#include <stdio.h>

#include <jansson.h>

#include "../core/request.c"

void ABOUT(const json_t *data) {
  char path[37];
  sprintf(path, "/channels/%s/messages", json_string_value(json_object_get(data, "channel_id")));
  request(path, "POST", "{"
    "\"embeds\": [{"
      "\"description\": \"Hi, I'm a discord bot.\""
    "}]"
  "}");
}
