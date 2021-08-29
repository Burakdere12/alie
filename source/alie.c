#include "./core/websocket.c"
#include "./configuration.c"

int main() {
  init_configuration();

  struct GatewaySettings gateway_settings;
  gateway_settings.token = config.token;
  gateway_settings.status = "dnd";
  gateway_settings.presence_text = "yuyo";
  gateway_settings.presence_type = 0;

  connect_websocket(gateway_settings);
  return 0;
}
