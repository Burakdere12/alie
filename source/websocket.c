#include <stdio.h>
#include <string.h>
#include <libwebsockets.h>

struct websocket_connection {
  lws_sorted_usec_list_t sul;
  struct lws *wsi;
};

static struct websocket_connection* wsc;
static struct lws_context* context;

static void prepare_websocket(lws_sorted_usec_list_t* sul) {
  struct websocket_connection* wsc = lws_container_of(sul, struct websocket_connection, sul);
  struct lws_client_connect_info info;

  memset(&info, 0, sizeof(info));

  info.context = context;
  info.port = 443;
  info.address = "gateway.discord.gg";
  info.path = "/?version=v9&encoding=json";
  info.host = info.address;
  info.origin = info.address;
  info.ssl_connection = LCCSCF_USE_SSL;
  info.protocol = "dumb-increment-protocol";
  info.local_protocol_name = "alie-bot";
  info.pwsi = &wsc->wsi;
  info.userdata = wsc;

  lws_client_connect_via_info(&info);
}

static int websocket_callback(struct lws* wsi, enum lws_callback_reasons reason, void* user, void *in, size_t len) {
  wsc = (struct websocket_connection*) user;

  switch (reason) {
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
      lwsl_err("CLIENT_CONNECTION_ERROR: %s\n", in ? (char*) in : "(null)");
      break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
      lwsl_hexdump_notice(in, len);
      break;

    case LWS_CALLBACK_CLIENT_ESTABLISHED:
      lwsl_user("%s: established\n", __func__);
      break;

    case LWS_CALLBACK_CLIENT_CLOSED:
      lwsl_err("%s: closed\n", __func__);
      break;

    default:
      break;
  }

  return lws_callback_http_dummy(wsi, reason, user, in, len);
}

static const struct lws_protocols protocols[] = {
  { "alie-bot", websocket_callback, 0, 0, 0, NULL, 0 },
  { NULL, NULL, 0, 0, 0, NULL, 0 }
};

void connect_websocket() {
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
