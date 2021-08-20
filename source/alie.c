#include "./websocket.c"
#include "./configuration.c"

int main() {
  init_configuration();
  connect_websocket();
  return 0;
}
