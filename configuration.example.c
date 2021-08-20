#include <string.h>

struct Configuration {
  char* token;
  char* developers[2];
  char* prefix;
} config;

void init_configuration() {
  config.token = "";
  config.developers[0] = "";
  config.developers[1] = "";
  config.prefix = "";
}
