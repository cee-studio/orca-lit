#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h> /* offsetof() */
#include <ctype.h> /* isspace() */


#include "websockets.h"
#include "discord-datatype.h"
#include "discord.h"
#include "discord-internal.h"


char* discord_gateway_opcodes_print(enum discord_gateway_opcodes v) {
  switch (v) {
    case DISCORD_GATEWAY_DISPATCH: return "DISPATCH";
    case DISCORD_GATEWAY_HEARTBEAT: return "HEARTBEAT";
    case DISCORD_GATEWAY_IDENTIFY: return "IDENTIFY";
    case DISCORD_GATEWAY_PRESENCE_UPDATE: return "PRESENCE_UPDATE";
    case DISCORD_GATEWAY_VOICE_STATE_UPDATE: return "VOICE_STATE_UPDATE";
    case DISCORD_GATEWAY_RESUME: return "RESUME";
    case DISCORD_GATEWAY_RECONNECT: return "RECONNECT";
    case DISCORD_GATEWAY_REQUEST_GUILD_MEMBERS: return "REQUEST_GUILD_MEMBERS";
    case DISCORD_GATEWAY_INVALID_SESSION: return "INVALID_SESSION";
    case DISCORD_GATEWAY_HELLO: return "HELLO";
    case DISCORD_GATEWAY_HEARTBEAT_ACK: return "HEARTBEAT_ACK";
  }
  return NULL;
}

enum discord_gateway_close_opcodes discord_gateway_close_opcodes_eval(char *s) {
  if(strcasecmp("CLOSE_REASON_UNKNOWN_ERROR", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_UNKNOWN_ERROR;
  if(strcasecmp("CLOSE_REASON_UNKNOWN_OPCODE", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_UNKNOWN_OPCODE;
  if(strcasecmp("CLOSE_REASON_DECODE_ERROR", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_DECODE_ERROR;
  if(strcasecmp("CLOSE_REASON_NOT_AUTHENTICATED", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_NOT_AUTHENTICATED;
  if(strcasecmp("CLOSE_REASON_AUTHENTICATION_FAILED", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_AUTHENTICATION_FAILED;
  if(strcasecmp("CLOSE_REASON_ALREADY_AUTHENTICATED", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_ALREADY_AUTHENTICATED;
  if(strcasecmp("CLOSE_REASON_INVALID_SEQUENCE", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_INVALID_SEQUENCE;
  if(strcasecmp("CLOSE_REASON_RATE_LIMITED", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_RATE_LIMITED;
  if(strcasecmp("CLOSE_REASON_SESSION_TIMED_OUT", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_SESSION_TIMED_OUT;
  if(strcasecmp("CLOSE_REASON_INVALID_SHARD", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_INVALID_SHARD;
  if(strcasecmp("CLOSE_REASON_SHARDING_REQUIRED", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_SHARDING_REQUIRED;
  if(strcasecmp("CLOSE_REASON_INVALID_API_VERSION", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_INVALID_API_VERSION;
  if(strcasecmp("CLOSE_REASON_INVALID_INTENTS", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_INVALID_INTENTS;
  if(strcasecmp("CLOSE_REASON_DISALLOWED_INTENTS", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_DISALLOWED_INTENTS;
  if(strcasecmp("CLOSE_REASON_RECONNECT", s) == 0) return DISCORD_GATEWAY_CLOSE_REASON_RECONNECT;
  ERR("'%s' doesn't match any known enumerator.", s);
  return -1;
}

char* discord_gateway_close_opcodes_print(enum discord_gateway_close_opcodes v) {
  switch (v) {
    case DISCORD_GATEWAY_CLOSE_REASON_UNKNOWN_ERROR: return "CLOSE_REASON_UNKNOWN_ERROR";
    case DISCORD_GATEWAY_CLOSE_REASON_UNKNOWN_OPCODE: return "CLOSE_REASON_UNKNOWN_OPCODE";
    case DISCORD_GATEWAY_CLOSE_REASON_DECODE_ERROR: return "CLOSE_REASON_DECODE_ERROR";
    case DISCORD_GATEWAY_CLOSE_REASON_NOT_AUTHENTICATED: return "CLOSE_REASON_NOT_AUTHENTICATED";
    case DISCORD_GATEWAY_CLOSE_REASON_AUTHENTICATION_FAILED: return "CLOSE_REASON_AUTHENTICATION_FAILED";
    case DISCORD_GATEWAY_CLOSE_REASON_ALREADY_AUTHENTICATED: return "CLOSE_REASON_ALREADY_AUTHENTICATED";
    case DISCORD_GATEWAY_CLOSE_REASON_INVALID_SEQUENCE: return "CLOSE_REASON_INVALID_SEQUENCE";
    case DISCORD_GATEWAY_CLOSE_REASON_RATE_LIMITED: return "CLOSE_REASON_RATE_LIMITED";
    case DISCORD_GATEWAY_CLOSE_REASON_SESSION_TIMED_OUT: return "CLOSE_REASON_SESSION_TIMED_OUT";
    case DISCORD_GATEWAY_CLOSE_REASON_INVALID_SHARD: return "CLOSE_REASON_INVALID_SHARD";
    case DISCORD_GATEWAY_CLOSE_REASON_SHARDING_REQUIRED: return "CLOSE_REASON_SHARDING_REQUIRED";
    case DISCORD_GATEWAY_CLOSE_REASON_INVALID_API_VERSION: return "CLOSE_REASON_INVALID_API_VERSION";
    case DISCORD_GATEWAY_CLOSE_REASON_INVALID_INTENTS: return "CLOSE_REASON_INVALID_INTENTS";
    case DISCORD_GATEWAY_CLOSE_REASON_DISALLOWED_INTENTS: return "CLOSE_REASON_DISALLOWED_INTENTS";
    case DISCORD_GATEWAY_CLOSE_REASON_RECONNECT: return "CLOSE_REASON_RECONNECT";
  }
  return NULL;
}

size_t discord_identify_connection_to_json(char *json, size_t len, struct discord_identify_connection *p)
{
  size_t r;
  void *arg_switches[3]={NULL};
  if (p->os != NULL)
    arg_switches[0] = p->os;

  if (p->browser != NULL)
    arg_switches[1] = p->browser;

  if (p->device != NULL)
    arg_switches[2] = p->device;

  r=json_inject(json, len,
                "($os):s,"
                "($browser):s,"
                "($device):s,"
                "@arg_switches:b",
                p->os,
                p->browser,
                p->device,
                arg_switches, sizeof(arg_switches), true);
  return r;
}

size_t discord_activity_to_json(char *json, size_t len, struct discord_activity *p)
{
  size_t r;
  void *arg_switches[8]={NULL};
  arg_switches[0] = p->name;
  arg_switches[1] = &p->type;
  if (p->url && *p->url)
    arg_switches[2] = p->url;

  if (p->created_at != 0)
    arg_switches[3] = &p->created_at;

  if (p->application_id != 0)
    arg_switches[4] = &p->application_id;

  if (p->details != NULL)
    arg_switches[5] = p->details;

  if (p->state != NULL)
    arg_switches[6] = p->state;

  if (p->instance != false)
    arg_switches[7] = &p->instance;

  r=json_inject(json, len,
                "(name):s,"
                "(type):d,"
                "(url):s,"
                "(created_at):|F|,"
                "(application_id):|F|,"
                "(details):s,"
                "(state):s,"
                "(instance):b,"
                "@arg_switches:b",
                p->name,
                &p->type,
                p->url,
                cee_unix_ms_to_iso8601, &p->created_at,
                cee_ulltostr, &p->application_id,
                p->details,
                p->state,
                &p->instance,
                arg_switches, sizeof(arg_switches), true);
  return r;
}

void discord_attachment_cleanup(struct discord_attachment *d) {
  if (d->content)
    free(d->content);
  (void)d->id;
  if (d->filename)
    free(d->filename);
  if (d->description)
    free(d->description);
  if (d->content_type)
    free(d->content_type);
  (void)d->size;
  if (d->url)
    free(d->url);
  if (d->proxy_url)
    free(d->proxy_url);
  (void)d->height;
  (void)d->width;
  (void)d->ephemeral;
}

void discord_attachment_list_free(struct discord_attachment **p) {
  ntl_free((void**)p, (void(*)(void*))discord_attachment_cleanup);
}


size_t discord_activity_list_to_json(char *str, size_t len, struct discord_activity **p) {
  return ntl_to_buf(str, len, (void **)p, NULL, (size_t(*)(char*,size_t,void*))discord_activity_to_json);
}

void cee_sized_buffer_from_json(char *str, size_t len, struct sized_buffer *buf) {
  buf->size = cee_strndup(str, len, &buf->start);
}

ORCAcode discord_get_gateway_bot(struct discord *client, struct sized_buffer *ret) {
  struct discord_request_attr attr = {
    ret, 0, NULL,
    (void (*)(char *, size_t, void *))cee_sized_buffer_from_json, NULL,
    NULL
  };

  return discord_adapter_run(&client->adapter, &attr, NULL, HTTP_GET,
                             "/gateway/bot");
}

size_t discord_presence_status_to_json(char *json, size_t len, struct discord_presence_status *p) {
  size_t r;
  void *arg_switches[4]={NULL};
  if (p->since != 0)
    arg_switches[0] = &p->since;

  if (p->activities != NULL)
    arg_switches[1] = p->activities;

  if (p->status != NULL)
    arg_switches[2] = p->status;

  arg_switches[3] = &p->afk;

  r=json_inject(json, len,
                "(since):|F|,"
                "(activities):F,"
                "(status):s,"
                "(afk):b,"
                "@arg_switches:b",
                cee_unix_ms_to_iso8601, &p->since,
                discord_activity_list_to_json, p->activities,
                p->status,
                &p->afk,
                arg_switches, sizeof(arg_switches), true);
  return r;
}


size_t discord_identify_to_json(char *json, size_t len, struct discord_identify *p) {
  size_t r;
  void *arg_switches[8]={NULL};
  arg_switches[0] = p->token;
  arg_switches[1] = p->properties;
  arg_switches[2] = &p->compress;
  arg_switches[3] = &p->large_threshold;
  arg_switches[4] = &p->guild_subscriptions;
  arg_switches[6] = p->presence;
  arg_switches[7] = &p->intents;

  r=json_inject(json, len,
                "(token):s,"
                "(properties):F,"
                "(compress):b,"
                "(large_threshold):d,"
                "(guild_subscriptions):b,"
                "(presence):F,"
                "(intents):d,"
                "@arg_switches:b",
                p->token,
                discord_identify_connection_to_json, p->properties,
                &p->compress,
                &p->large_threshold,
                &p->guild_subscriptions,
                discord_presence_status_to_json, p->presence,
                &p->intents,
                arg_switches, sizeof(arg_switches), true);
  return r;
}

void discord_session_start_limit_from_json(char *json, size_t len, struct discord_session_start_limit *p) {
  memset(p, 0, sizeof(struct discord_session_start_limit));
  json_extract(json, len,
               "(total):d,"
               "(remaining):d,"
               "(reset_after):d,"
               "(max_concurrency):d,",
               &p->total,
               &p->remaining,
               &p->reset_after,
               &p->max_concurrency);
}


/* shorten event callback for maintainability purposes */
#define ON(event, ...) gw->cmds.cbs.on_##event(CLIENT(gw, gw), __VA_ARGS__)

static const char* opcode_print(enum discord_gateway_opcodes opcode) {
  const char *str = discord_gateway_opcodes_print(opcode);
  if (!str) str = "Invalid Gateway opcode";
  return str;
}

static const char* close_opcode_print(enum discord_gateway_close_opcodes opcode) {
  const char *str;
  str = discord_gateway_close_opcodes_print(opcode);
  if (str) return str;

  str = ws_close_opcode_print((enum ws_close_reason)opcode);
  if (str) return str;
  return "Unknown WebSockets close opcode";
}


static void send_resume(struct discord_gateway *gw) {
  char buf[1024];
  size_t len;
  struct ws_info info = { 0 };

  /* reset */
  gw->session->status ^= DISCORD_SESSION_RESUMABLE;

  len = json_inject(buf, sizeof(buf),
                    "(op):6" /* RESUME OPCODE */
                    "(d):{"
                    "(token):s"
                    "(session_id):s"
                    "(seq):d"
                    "}",
                    gw->id.token, gw->session->id, &gw->payload.seq);
  ASSERT_S(len < sizeof(buf), "Out of bounds write attempt");
  ws_send_text(gw->ws, &info, buf, len);

  logconf_info(
    &gw->conf,
    ANSICOLOR("SEND", ANSI_FG_BRIGHT_GREEN) " RESUME (%d bytes) [@@@_%zu_@@@]",
    len, info.loginfo.counter + 1);
}

static void send_identify(struct discord_gateway *gw) {
  char buf[1024];
  size_t len;
  struct ws_info info = { 0 };

  /* Ratelimit check */
  if (gw->timer->now - gw->timer->identify < 5) {
    ++gw->session->concurrent;
    VASSERT_S(gw->session->concurrent
                < gw->session->start_limit.max_concurrency,
              "Reach identify request threshold (%d every 5 seconds)",
              gw->session->start_limit.max_concurrency);
  }
  else {
    gw->session->concurrent = 0;
  }

  len = json_inject(buf, sizeof(buf),
                    "(op):2" /* IDENTIFY OPCODE */
                    "(d):F",
                    &discord_identify_to_json, &gw->id);
  ASSERT_S(len < sizeof(buf), "Out of bounds write attempt");

  ws_send_text(gw->ws, &info, buf, len);

  logconf_info(
    &gw->conf,
    ANSICOLOR("SEND",
              ANSI_FG_BRIGHT_GREEN) " IDENTIFY (%d bytes) [@@@_%zu_@@@]",
    len, info.loginfo.counter + 1);

  /* get timestamp for this identify */
  gw->timer->identify = gw->timer->now;
}

static void on_reconnect(struct discord_gateway *);
/* send heartbeat pulse to websockets server in order
 *  to maintain connection alive */
static void send_heartbeat(struct discord_gateway *gw) {
  char buf[64];
  size_t len;
  struct ws_info info = { 0 };
  if (gw->timer->hbeat_ack_count < 0) {
    logconf_error(&gw->conf, "haven't received heartbeat_ack for %d time, reconnect.\n",
                  -gw->timer->hbeat_ack_count);
    /* a hack to work around the problem of not receiving heartbeat_acks */
    gw->timer->hbeat_ack_count = 0;
    on_reconnect(gw);
    return;
  }

  len = json_inject(buf, sizeof(buf), "(op):1,(d):d", &gw->payload.seq);
  ASSERT_S(len < sizeof(buf), "Out of bounds write attempt");

  ws_send_text(gw->ws, &info, buf, len);

  logconf_info(
    &gw->conf,
    ANSICOLOR("SEND",
              ANSI_FG_BRIGHT_GREEN) " HEARTBEAT (%d bytes) [@@@_%zu_@@@]",
    len, info.loginfo.counter + 1);

  /* update heartbeat timestamp */
  gw->timer->hbeat = gw->timer->now;
  gw->timer->hbeat_ack_count--;
}

static void on_hello(struct discord_gateway *gw) {
  gw->timer->interval = 0;
  gw->timer->hbeat = gw->timer->now;

  json_extract(gw->payload.data.start, gw->payload.data.size,
               "(heartbeat_interval):ld", &gw->timer->interval);
  logconf_info(&gw->conf, "heartbeat_interval %ld", gw->timer->interval);

  if (gw->session->status & DISCORD_SESSION_RESUMABLE)
    send_resume(gw);
  else
    send_identify(gw);
}

static enum discord_gateway_events get_dispatch_event(char name[]) {
#define RETURN_IF_MATCH(event, str)                                           \
  if (STREQ(#event, str)) return DISCORD_GATEWAY_EVENTS_##event

  RETURN_IF_MATCH(READY, name);
  RETURN_IF_MATCH(RESUMED, name);
  RETURN_IF_MATCH(MESSAGE_CREATE, name);
  RETURN_IF_MATCH(MESSAGE_UPDATE, name);
  RETURN_IF_MATCH(MESSAGE_DELETE, name);
  return DISCORD_GATEWAY_EVENTS_NONE;

#undef RETURN_IF_MATCH
}

static void on_message_create(struct discord_gateway *gw, struct sized_buffer *data) {
  /* handle message here */
  if (gw->cmds.cbs.on_message_create)
    gw->cmds.cbs.on_message_create(CLIENT(gw, gw), (void *)data);
}

static void on_message_update(struct discord_gateway *gw, struct sized_buffer *data) {
  if (gw->cmds.cbs.on_message_update)
    gw->cmds.cbs.on_message_update(CLIENT(gw, gw), (void *)data);
}

static void on_message_delete(struct discord_gateway *gw, struct sized_buffer *data) {
  if (gw->cmds.cbs.on_message_delete)
    gw->cmds.cbs.on_message_delete(CLIENT(gw, gw), (void *)data);
}

static void on_message_delete_bulk(struct discord_gateway *gw, struct sized_buffer *data) {
  if (gw->cmds.cbs.on_message_delete_bulk)
    gw->cmds.cbs.on_message_delete_bulk(CLIENT(gw, gw), (void *)data);
}

static void on_ready(struct discord_gateway *gw, struct sized_buffer *data) {
  (void)data;
  gw->cmds.cbs.on_ready(CLIENT(gw, gw));
}

struct discord * discord_clone(const struct discord *orig_client) {
  struct discord *clone_client = malloc(sizeof(struct discord));

  memcpy(clone_client, orig_client, sizeof(struct discord));
  clone_client->is_original = false;

  return clone_client;
}

static void dispatch_run(void *p_cxt) {
  struct discord_event *cxt = p_cxt;
  struct discord *client = CLIENT(cxt->gw, gw);

  logconf_info(&cxt->gw->conf,
               "Thread " ANSICOLOR("starts", ANSI_FG_RED) " to serve %s",
               cxt->name);

  cxt->on_event(cxt->gw, &cxt->data);

  logconf_info(&cxt->gw->conf,
               "Thread " ANSICOLOR("exits", ANSI_FG_RED) " from serving %s",
               cxt->name);

  /* TODO: move to _discord_event_cleanup() */
  free(cxt->name);
  free(cxt->data.start);
  discord_cleanup(client);
  free(cxt);
}

static void on_dispatch(struct discord_gateway *gw) {
  struct discord *client = CLIENT(gw, gw);

  /* event-callback selector */
  void (*on_event)(struct discord_gateway *, struct sized_buffer *) = NULL;
  /* get dispatch event opcode */
  enum discord_gateway_events event;
  enum discord_event_scheduler mode;

  /* TODO: this should only apply for user dispatched payloads? */
#if 0
  /* Ratelimit check */
  if (gw->timer->now - gw->timer->event < 60000) {
    ++gw->session->event_count;
    ASSERT_S(gw->session->event_count < 120,
             "Reach event dispatch threshold (120 every 60 seconds)");
  }
  else {
    gw->timer->event = gw->timer->now;
    gw->session->event_count = 0;
  }
#endif

  switch (event = get_dispatch_event(gw->payload.name)) {
    case DISCORD_GATEWAY_EVENTS_READY:
      logconf_info(&gw->conf, "Succesfully started a Discord session!");

      json_extract(gw->payload.data.start, gw->payload.data.size,
                   "(session_id):s", gw->session->id);
      ASSERT_S(!IS_EMPTY_STRING(gw->session->id),
               "Missing session_id from READY event");

      gw->session->is_ready = true;
      gw->session->retry.attempt = 0;
      if (gw->cmds.cbs.on_ready)
        on_event = on_ready;
      send_heartbeat(gw);
      break;
    case DISCORD_GATEWAY_EVENTS_RESUMED:
      logconf_info(&gw->conf, "Succesfully resumed a Discord session!");

      gw->session->is_ready = true;
      gw->session->retry.attempt = 0;
      send_heartbeat(gw);
      break;
    case DISCORD_GATEWAY_EVENTS_MESSAGE_CREATE:
      if (gw->cmds.cbs.on_message_create)
        on_event = on_message_create;
      break;
    case DISCORD_GATEWAY_EVENTS_MESSAGE_UPDATE:
      if (gw->cmds.cbs.on_message_update)
        on_event = on_message_update;
      break;
    case DISCORD_GATEWAY_EVENTS_MESSAGE_DELETE:
      if (gw->cmds.cbs.on_message_delete)
        on_event = on_message_delete;
      break;
    default:
      logconf_warn(&gw->conf,
                   "Unimplemented GATEWAY_DISPATCH event %s (code: %d)",
                   gw->payload.name, event);
      break;
  }

  mode = gw->cmds.scheduler(client, &gw->payload.data, event);
  if (!on_event) return;

  /* user subscribed to event */
  switch (mode) {
    case DISCORD_EVENT_IGNORE:
      break;
    case DISCORD_EVENT_MAIN_THREAD:
      on_event(gw, &gw->payload.data);
      break;
    case DISCORD_EVENT_WORKER_THREAD: {
      struct discord_event *cxt = malloc(sizeof *cxt);
      int ret;

      cxt->name = strdup(gw->payload.name);
      cxt->gw = &(discord_clone(client)->gw);
      cxt->data.size = cee_strndup(gw->payload.data.start, gw->payload.data.size,
                                   &cxt->data.start);
      cxt->event = event;
      cxt->on_event = on_event;

      ret = work_run(&dispatch_run, cxt);
      VASSERT_S(0 == ret, "Couldn't create task (code %d)", ret);
    }
      break;
    default:
      ERR("Unknown event handling mode (code: %d)", mode);
  }
}

static void on_invalid_session(struct discord_gateway *gw) {
  enum ws_close_reason opcode;
  const char *reason;

  gw->session->status = DISCORD_SESSION_SHUTDOWN;
  if (0 != strncmp(gw->payload.data.start, "false", gw->payload.data.size)) {
    gw->session->status |= DISCORD_SESSION_RESUMABLE;
    reason = "Invalid session, will attempt to resume";
    opcode = (enum ws_close_reason)DISCORD_GATEWAY_CLOSE_REASON_RECONNECT;
  }
  else {
    reason = "Invalid session, can't resume";
    opcode = WS_CLOSE_REASON_NORMAL;
  }
  gw->session->retry.enable = true;

  ws_close(gw->ws, opcode, reason, SIZE_MAX);
}

static void on_reconnect(struct discord_gateway *gw) {
  const char reason[] = "Discord expects client to reconnect";

  gw->session->status = DISCORD_SESSION_RESUMABLE | DISCORD_SESSION_SHUTDOWN;
  gw->session->retry.enable = true;
  ws_close(gw->ws,
           (enum ws_close_reason)DISCORD_GATEWAY_CLOSE_REASON_RECONNECT,
           reason, sizeof(reason));
}

static void on_heartbeat_ack(struct discord_gateway *gw) {
  /* get request / response interval in milliseconds */
  pthread_rwlock_wrlock(&gw->timer->rwlock);
  gw->timer->ping_ms = gw->timer->now - gw->timer->hbeat;
  pthread_rwlock_unlock(&gw->timer->rwlock);
  gw->timer->hbeat_ack_count++;
  logconf_info(&gw->conf, "HEARTBEAT_ACK: %d ms", gw->timer->ping_ms);
}

static void on_connect_cb(void *p_gw,
                          struct websockets *ws,
                          struct ws_info *info,
                          const char *ws_protocols) {
  struct discord_gateway *gw = p_gw;
  (void)ws;
  (void)info;

  logconf_info(&gw->conf, "Connected, WS-Protocols: '%s'", ws_protocols);
}

static void on_close_cb(void *p_gw,
                        struct websockets *ws,
                        struct ws_info *info,
                        enum ws_close_reason wscode,
                        const char *reason,
                        size_t len) {
  struct discord_gateway *gw = p_gw;
  enum discord_gateway_close_opcodes opcode =
    (enum discord_gateway_close_opcodes)wscode;
  (void)ws;
  (void)info;

  logconf_warn(
    &gw->conf,
    ANSICOLOR("CLOSE %s", ANSI_FG_RED) " (code: %4d, %zu bytes): '%.*s'",
    close_opcode_print(opcode), opcode, len, (int)len, reason);

  /* user-triggered shutdown */
  if (gw->session->status & DISCORD_SESSION_SHUTDOWN) return;

  /* mark as in the process of being shutdown */
  gw->session->status |= DISCORD_SESSION_SHUTDOWN;

  switch (opcode) {
  case DISCORD_GATEWAY_CLOSE_REASON_UNKNOWN_ERROR:
  case DISCORD_GATEWAY_CLOSE_REASON_INVALID_SEQUENCE:
  case DISCORD_GATEWAY_CLOSE_REASON_UNKNOWN_OPCODE:
  case DISCORD_GATEWAY_CLOSE_REASON_DECODE_ERROR:
  case DISCORD_GATEWAY_CLOSE_REASON_NOT_AUTHENTICATED:
  case DISCORD_GATEWAY_CLOSE_REASON_AUTHENTICATION_FAILED:
  case DISCORD_GATEWAY_CLOSE_REASON_ALREADY_AUTHENTICATED:
  case DISCORD_GATEWAY_CLOSE_REASON_RATE_LIMITED:
  case DISCORD_GATEWAY_CLOSE_REASON_SHARDING_REQUIRED:
  case DISCORD_GATEWAY_CLOSE_REASON_INVALID_API_VERSION:
  case DISCORD_GATEWAY_CLOSE_REASON_INVALID_INTENTS:
  case DISCORD_GATEWAY_CLOSE_REASON_INVALID_SHARD:
  case DISCORD_GATEWAY_CLOSE_REASON_DISALLOWED_INTENTS:
    gw->session->status &= ~DISCORD_SESSION_RESUMABLE;
    gw->session->retry.enable = false;
    break;
  default: /*websocket/clouflare opcodes */
    if (WS_CLOSE_REASON_NORMAL == (enum ws_close_reason)opcode) {
      gw->session->status |= DISCORD_SESSION_RESUMABLE;
      gw->session->retry.enable = false;
    }
    else {
      logconf_warn(
        &gw->conf,
        "Gateway will attempt to reconnect and start a new session");
      gw->session->status &= ~DISCORD_SESSION_RESUMABLE;
      gw->session->retry.enable = true;
    }
    break;
  case DISCORD_GATEWAY_CLOSE_REASON_SESSION_TIMED_OUT:
    logconf_warn(
      &gw->conf,
      "Gateway will attempt to reconnect and resume current session");
    gw->session->status &= ~DISCORD_SESSION_RESUMABLE;
    gw->session->retry.enable = true;
    break;
  }
}

static void on_text_cb(void *p_gw,
                       struct websockets *ws,
                       struct ws_info *info,
                       const char *text,
                       size_t len) {
  struct discord_gateway *gw = p_gw;
  /* check sequence value first, then assign */
  int seq = 0;
  (void)ws;

  json_extract((char *)text, len, "(t):s (s):d (op):d (d):T", gw->payload.name,
               &seq, &gw->payload.opcode, &gw->payload.data);

  if (seq) gw->payload.seq = seq;

  logconf_trace(
    &gw->conf,
    ANSICOLOR("RCV",
              ANSI_FG_BRIGHT_YELLOW) " %s%s%s (%zu bytes) [@@@_%zu_@@@]",
    opcode_print(gw->payload.opcode), *gw->payload.name ? " -> " : "",
    gw->payload.name, len, info->loginfo.counter);

  switch (gw->payload.opcode) {
  case DISCORD_GATEWAY_DISPATCH:
    on_dispatch(gw);
    break;
  case DISCORD_GATEWAY_INVALID_SESSION:
    on_invalid_session(gw);
    break;
  case DISCORD_GATEWAY_RECONNECT:
    on_reconnect(gw);
    break;
  case DISCORD_GATEWAY_HELLO:
    on_hello(gw);
    break;
  case DISCORD_GATEWAY_HEARTBEAT_ACK:
    on_heartbeat_ack(gw);
    break;
  default:
    logconf_error(&gw->conf, "Not yet implemented Gateway Event (code: %d)",
                  gw->payload.opcode);
    break;
  }
}

static discord_event_scheduler_t default_scheduler_cb(struct discord *a,
                                                      struct sized_buffer *b,
                                                      enum discord_gateway_events c) {
  (void)a;
  (void)b;
  (void)c;
  return DISCORD_EVENT_MAIN_THREAD;
}


void
discord_gateway_init(struct discord_gateway *gw,
                     struct logconf *conf,
                     struct sized_buffer *token) {
  struct discord *client = CLIENT(gw, gw);
  /* Web-Sockets callbacks */
  struct ws_callbacks cbs = { 0 };
  /* Web-Sockets custom attributes */
  struct ws_attr attr = { 0 };
  /* Bot default presence status */
  struct discord_presence_status presence = { 0 };
  struct sized_buffer buf;

  cbs.data = gw;
  cbs.on_connect = &on_connect_cb;
  cbs.on_text = &on_text_cb;
  cbs.on_close = &on_close_cb;

  attr.conf = conf;

  /* Web-Sockets handler */
  gw->mhandle = curl_multi_init();
  gw->ws = ws_init(&cbs, gw->mhandle, &attr);
  logconf_branch(&gw->conf, conf, "DISCORD_GATEWAY");

  gw->timer = calloc(1, sizeof *gw->timer);
  if (pthread_rwlock_init(&gw->timer->rwlock, NULL))
    ERR("Couldn't initialize pthread rwlock");

  /* client connection status */
  gw->session = calloc(1, sizeof *gw->session);
  gw->session->retry.enable = true;
  gw->session->retry.limit = 5; /**< hard limit for now */

  /* connection identify token */
  cee_strndup(token->start, token->size, &gw->id.token);

  /* connection identify properties */
  gw->id.properties = calloc(1, sizeof *gw->id.properties);
  gw->id.properties->os = "POSIX";
  gw->id.properties->browser = "orca";
  gw->id.properties->device = "orca";

  /* the bot initial presence */
  gw->id.presence = calloc(1, sizeof *gw->id.presence);
  presence.status = "online";
  presence.since = cee_timestamp_ms();
  /*discord_set_presence(client, &presence);*/

  /* default callbacks */
  gw->cmds.scheduler = default_scheduler_cb;

  /* check for default prefix in config file */
  buf = logconf_get_field(conf, "discord.default_prefix");
  if (buf.size) {
    bool enable_prefix = false;
    json_extract(buf.start, buf.size, "(enable):b", &enable_prefix);

    if (enable_prefix) {
      char *prefix = NULL;
      json_extract(buf.start, buf.size, "(prefix):?s", &prefix);

      gw->cmds.prefix.start = prefix;
      gw->cmds.prefix.size = prefix ? strlen(prefix) : 0;
    }
  }
}

void discord_gateway_cleanup(struct discord_gateway *gw) {
  /* cleanup WebSockets handle */
  curl_multi_cleanup(gw->mhandle);
  ws_cleanup(gw->ws);
  /* cleanup timers */
  pthread_rwlock_destroy(&gw->timer->rwlock);
  free(gw->timer);
  /* cleanup bot identification */
  if (gw->id.token) free(gw->id.token);
  free(gw->id.properties);
  free(gw->id.presence);
  /* cleanup client session */
  free(gw->session);
  /* cleanup user commands */
  if (gw->cmds.pool) free(gw->cmds.pool);
  if (gw->cmds.prefix.start) free(gw->cmds.prefix.start);
}



ORCAcode discord_gateway_start(struct discord_gateway *gw) {
  struct discord *client = CLIENT(gw, gw);
  /* get gateway bot info */
  struct sized_buffer json = { 0 };
  /* build URL that will be used to connect to Discord */
  char *base_url, url[1024];
  /* snprintf() OOB check */
  size_t len;

  if (gw->session->retry.attempt >= gw->session->retry.limit) {
    logconf_fatal(&gw->conf, "Failed reconnecting to Discord after %d tries",
                  gw->session->retry.limit);
    return ORCA_DISCORD_CONNECTION;
  }

  if (discord_get_gateway_bot(client, &json)) {
    logconf_fatal(&gw->conf, "Couldn't retrieve Gateway Bot information");
    return ORCA_DISCORD_BAD_AUTH;
  }

  extern void discord_session_start_limit_from_json(char *json, size_t len, struct discord_session_start_limit *p);
  json_extract(json.start, json.size,
               "(url):?s,(shards):d,(session_start_limit):F", &base_url,
               &gw->session->shards, &discord_session_start_limit_from_json,
               &gw->session->start_limit);

  len = snprintf(url, sizeof(url), "%s%s" DISCORD_GATEWAY_URL_SUFFIX, base_url,
                 ('/' == base_url[strlen(base_url) - 1]) ? "" : "/");
  ASSERT_S(len < sizeof(url), "Out of bounds write attempt");

  free(json.start);
  free(base_url);

  if (!gw->session->start_limit.remaining) {
    logconf_fatal(&gw->conf,
                  "Reach sessions threshold (%d),"
                  "Please wait %d seconds and try again",
                  gw->session->start_limit.total,
                  gw->session->start_limit.reset_after / 1000);

    return ORCA_DISCORD_RATELIMIT;
  }

  ws_set_url(gw->ws, url, NULL);
  ws_start(gw->ws);

  return ORCA_OK;
}

bool discord_gateway_end(struct discord_gateway *gw) {
  ws_end(gw->ws);

  /* keep only resumable information */
  gw->session->status &= DISCORD_SESSION_RESUMABLE;
  gw->session->is_ready = false;

  if (!gw->session->retry.enable) {
    logconf_warn(&gw->conf, "Discord Gateway Shutdown");

    /* reset for next run */
    gw->session->status = DISCORD_SESSION_OFFLINE;
    gw->session->is_ready = false;
    gw->session->retry.enable = false;
    gw->session->retry.attempt = 0;

    return true;
  }

  ++gw->session->retry.attempt;

  logconf_info(&gw->conf, "Reconnect attempt #%d", gw->session->retry.attempt);

  return false;
}

ORCAcode discord_gateway_perform(struct discord_gateway *gw) {
  /* check for pending transfer, exit on failure */
  if (!ws_easy_run(gw->ws, 5, &gw->timer->now))
    return ORCA_DISCORD_CONNECTION;

  /* client is in the process of shutting down */
  if (gw->session->status & DISCORD_SESSION_SHUTDOWN)
    return ORCA_OK;

  /* client is in the process of connecting */
  if (!gw->session->is_ready)
    return ORCA_OK;

  /* check if timespan since first pulse is greater than
   * minimum heartbeat interval required */
  if (gw->timer->interval < gw->timer->now - gw->timer->hbeat)
    send_heartbeat(gw);

  if (gw->cmds.cbs.on_idle)
    gw->cmds.cbs.on_idle(CLIENT(gw, gw));

  return ORCA_OK;
}

void discord_gateway_shutdown(struct discord_gateway *gw) {
  const char reason[] = "Client triggered shutdown";

  /* TODO: MT-Unsafe section */
  gw->session->retry.enable = false;
  gw->session->status = DISCORD_SESSION_SHUTDOWN;

  ws_close(gw->ws, WS_CLOSE_REASON_NORMAL, reason, sizeof(reason));
}

void discord_gateway_reconnect(struct discord_gateway *gw, bool resume) {
  const char reason[] = "Client triggered reconnect";
  enum ws_close_reason opcode;

  /* TODO: MT-Unsafe section */
  gw->session->retry.enable = true;
  gw->session->status = DISCORD_SESSION_SHUTDOWN;
  if (resume) {
    gw->session->status |= DISCORD_SESSION_RESUMABLE;
    opcode = (enum ws_close_reason)DISCORD_GATEWAY_CLOSE_REASON_RECONNECT;
  }
  else {
    opcode = WS_CLOSE_REASON_NORMAL;
  }

  ws_close(gw->ws, opcode, reason, sizeof(reason));
}
