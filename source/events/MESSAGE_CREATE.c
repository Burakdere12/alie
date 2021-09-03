#include <stdbool.h>

#include <jansson.h>

#include "../../configuration.c"

// :(
#include "../commands/about.c"

void MESSAGE_CREATE(const json_t *data) {
  const char *content = json_string_value(json_object_get(data, "content"));

  size_t prefix_length = strlen(config.prefix);

  bool has_prefix = true;
  short i = 0;

  while (i < (short) prefix_length) {
    if (config.prefix[i] != content[i]) {
      has_prefix = false;
      break;
    }

    i += 1;
  }

  if (has_prefix) {
    i = 0;
    size_t content_length = strlen(content);
    size_t offset_length = content_length;
    char command[content_length - prefix_length];

    while (offset_length != prefix_length) {
      command[content_length - offset_length] = content[content_length - offset_length + prefix_length];
      offset_length -= 1;
    }

    command[content_length - prefix_length] = '\0';

    if (strcmp(command, "about") == 0) {
      ABOUT(data);
    }
  }
}
