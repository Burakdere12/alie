#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <libwebsockets.h>
#include <jansson.h>
#include <unistd.h>
#include <pthread.h>

#include "./store.c"
#include "../tools/logs.c"

// sorry for this :(
#include "../events/READY.c"
#include "../events/GUILD_CREATE.c"
#include "../events/GUILD_DELETE.c"
#include "../events/MESSAGE_CREATE.c"

struct GatewaySettings {
  char *token;
  char *status;
  char *presence_text;
  short presence_type;
};

struct websocket_connection {
  lws_sorted_usec_list_t sul;
  struct lws *wsi;
};

static struct websocket_connection *wsc;
static struct lws_context *context;
static struct GatewaySettings gateway_settings;
static short last_sequence = -1;
static unsigned short heartbeat_interval;
static char *payload;
static size_t payload_size, waiting_guilds = 0;
static pthread_t heartbeat_thread;

void *send_heartbeat() {
  while (true) {
    sleep(heartbeat_interval);
    // TODO: send heartbeat
  }

  return NULL;
}

static void prepare_websocket(lws_sorted_usec_list_t *sul) {
  struct websocket_connection *wsc = lws_container_of(sul, struct websocket_connection, sul);
  struct lws_client_connect_info info;

  memset(&info, 0, sizeof(info));

  info.context = context;
  info.port = 443;
  info.address = "gateway.discord.gg";
  info.path = "/?v=9&encoding=json";
  info.host = info.address;
  info.origin = info.address;
  info.ssl_connection = LCCSCF_USE_SSL;
  info.protocol = "dumb-increment-protocol";
  info.local_protocol_name = "alie-bot";
  info.pwsi = &wsc->wsi;
  info.userdata = wsc;

  lws_client_connect_via_info(&info);
}

static int websocket_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
  wsc = (struct websocket_connection*) user;

  if (reason == LWS_CALLBACK_CLIENT_CONNECTION_ERROR) lwsl_err("CLIENT_CONNECTION_ERROR: %s\n", in ? (char*) in : "(null)");
  else if (reason == LWS_CALLBACK_CLIENT_RECEIVE) {
    const json_t *root = json_loads(in, 0, NULL);

    const int op_code = json_number_value(json_object_get(root, "op"));
    const json_t *data = json_object_get(root, "d");
    logs(DEBUG, INFO, "Received %d operation code.", op_code);

    if (op_code == 10) {
      heartbeat_interval = json_number_value(json_object_get(data, "heartbeat_interval"));

      pthread_create(&heartbeat_thread, NULL, &send_heartbeat, NULL);
      pthread_detach(heartbeat_thread);
      logs(DEBUG, INFO, "Heartbeat sending thread is created.");

      if (gateway_settings.presence_text != NULL) {
        payload_size = sprintf(payload, "{"
          "\"op\": 2,"
          "\"d\": {"
            "\"token\": \"%s\","
            "\"intents\": 32767,"
            "\"properties\": {"
              "\"$os\": \"linux\","
              "\"$browser\": \"alie-bot\","
              "\"$device\": \"alie-bot\""
            "},"
            "\"presence\": {"
              "\"activities\": [{"
                "\"name\": \"%s\","
                "\"type\": %d"
              "}],"
              "\"status\": \"dnd\","
              "\"afk\": false"
            "}"
          "}"
        "}", gateway_settings.token, gateway_settings.presence_text, gateway_settings.presence_type);
      } else {
        payload_size = sprintf(payload, "{"
          "\"op\": 2,"
          "\"d\": {"
            "\"token\": \"%s\","
            "\"intents\": 0,"
            "\"properties\": {"
              "\"$os\": \"linux\","
              "\"$browser\": \"alie-bot\","
              "\"$device\": \"alie-bot\""
            "}"
          "}"
        "}", gateway_settings.token);
      }

      logs(DEBUG, INFO, "Identify payload sent.");
      lws_callback_on_writable(wsi);
    } else if (op_code == 0) {
      const char *event_name = json_string_value(json_object_get(root, "t"));
      last_sequence = json_number_value(json_object_get(root, "s"));
      logs(DEBUG, INFO, "%s event received.", event_name);

      if (strcmp(event_name, "READY") == 0) {
        store.user = json_object_get(data, "user");
        waiting_guilds = json_object_size(json_object_get(data, "guilds"));
        if (waiting_guilds == 0) READY();
      } else if (strcmp(event_name, "GUILD_CREATE") == 0) {
        store.guild_count += 1;

        if (waiting_guilds == 0) GUILD_CREATE(data);
        else {
          waiting_guilds -= 1;
          if (waiting_guilds == 0) READY();
        }
      } else if (strcmp(event_name, "GUILD_DELETE") == 0) {
        store.guild_count -= 1;
        GUILD_DELETE(data);
      } else if (strcmp(event_name, "MESSAGE_CREATE") == 0) MESSAGE_CREATE(data);
    }
  } else if (reason == LWS_CALLBACK_CLIENT_WRITEABLE) lws_write(wsi, (unsigned char*) payload, payload_size, LWS_WRITE_TEXT);
  else if (reason == LWS_CALLBACK_CLIENT_ESTABLISHED) lwsl_user("%s: established\n", __func__);
  else if (reason == LWS_CALLBACK_CLIENT_CLOSED) lwsl_err("%s: closed\n", __func__);

  return lws_callback_http_dummy(wsi, reason, user, in, len);
}

static const struct lws_protocols protocols[] = {
  { "alie-bot", websocket_callback, 0, 12288, 0, NULL, 0 }, // 12288 = Max content size, 1024 * 12
  { NULL, NULL, 0, 0, 0, NULL, 0 }
};

void connect_websocket(struct GatewaySettings settings) {
  gateway_settings = settings;
  store.token = settings.token;
  payload = malloc(1536); // 1024 * 1.5

  struct lws_context_creation_info info;
  memset(&info, 0, sizeof info);

  info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
  info.port = CONTEXT_PORT_NO_LISTEN;
  info.protocols = protocols;

  info.fd_limit_per_thread = 3;

  context = lws_create_context(&info);

  if (!context) {
    lwsl_err("LWS couldn't setup.\n");
    return;
  }

  prepare_websocket(&wsc->sul);

  int n = 0;
  while (n >= 0) n = lws_service(context, 0);

  lws_context_destroy(context);
  lwsl_user("Connection is closed.\n");
}
