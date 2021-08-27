#include "./websocket.c"
#include "./configuration.c"

int main() {
  init_configuration();
  connect_websocket(config.token);
  return 0;
}
