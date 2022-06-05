#ifndef DISCORD_DATATYPE_H
#define DISCORD_DATATYPE_H

enum discord_gateway_opcodes {
  DISCORD_GATEWAY_DISPATCH = 0,
  DISCORD_GATEWAY_HEARTBEAT = 1,
  DISCORD_GATEWAY_IDENTIFY = 2,
  DISCORD_GATEWAY_PRESENCE_UPDATE = 3,
  DISCORD_GATEWAY_VOICE_STATE_UPDATE = 4,
  DISCORD_GATEWAY_RESUME = 6,
  DISCORD_GATEWAY_RECONNECT = 7,
  DISCORD_GATEWAY_REQUEST_GUILD_MEMBERS = 8,
  DISCORD_GATEWAY_INVALID_SESSION = 9,
  DISCORD_GATEWAY_HELLO = 10,
  DISCORD_GATEWAY_HEARTBEAT_ACK = 11,
};
extern char* discord_gateway_opcodes_print(enum discord_gateway_opcodes);
extern enum discord_gateway_opcodes discord_gateway_opcodes_eval(char*);
extern void discord_gateway_opcodes_list_free_v(void **p);
extern void discord_gateway_opcodes_list_free(enum discord_gateway_opcodes **p);
extern void discord_gateway_opcodes_list_from_json_v(char *str, size_t len, void *p);
extern void discord_gateway_opcodes_list_from_json(char *str, size_t len, enum discord_gateway_opcodes ***p);
extern size_t discord_gateway_opcodes_list_to_json_v(char *str, size_t len, void *p);
extern size_t discord_gateway_opcodes_list_to_json(char *str, size_t len, enum discord_gateway_opcodes **p);

enum discord_gateway_close_opcodes {
  DISCORD_GATEWAY_CLOSE_REASON_UNKNOWN_ERROR = 4000,
  DISCORD_GATEWAY_CLOSE_REASON_UNKNOWN_OPCODE = 4001,
  DISCORD_GATEWAY_CLOSE_REASON_DECODE_ERROR = 4002,
  DISCORD_GATEWAY_CLOSE_REASON_NOT_AUTHENTICATED = 4003,
  DISCORD_GATEWAY_CLOSE_REASON_AUTHENTICATION_FAILED = 4004,
  DISCORD_GATEWAY_CLOSE_REASON_ALREADY_AUTHENTICATED = 4005,
  DISCORD_GATEWAY_CLOSE_REASON_INVALID_SEQUENCE = 4007,
  DISCORD_GATEWAY_CLOSE_REASON_RATE_LIMITED = 4008,
  DISCORD_GATEWAY_CLOSE_REASON_SESSION_TIMED_OUT = 4009,
  DISCORD_GATEWAY_CLOSE_REASON_INVALID_SHARD = 4010,
  DISCORD_GATEWAY_CLOSE_REASON_SHARDING_REQUIRED = 4011,
  DISCORD_GATEWAY_CLOSE_REASON_INVALID_API_VERSION = 4012,
  DISCORD_GATEWAY_CLOSE_REASON_INVALID_INTENTS = 4013,
  DISCORD_GATEWAY_CLOSE_REASON_DISALLOWED_INTENTS = 4014,
  DISCORD_GATEWAY_CLOSE_REASON_RECONNECT = 4900,
};
extern char* discord_gateway_close_opcodes_print(enum discord_gateway_close_opcodes);
extern enum discord_gateway_close_opcodes discord_gateway_close_opcodes_eval(char*);
extern void discord_gateway_close_opcodes_list_free_v(void **p);
extern void discord_gateway_close_opcodes_list_free(enum discord_gateway_close_opcodes **p);
extern void discord_gateway_close_opcodes_list_from_json_v(char *str, size_t len, void *p);
extern void discord_gateway_close_opcodes_list_from_json(char *str, size_t len, enum discord_gateway_close_opcodes ***p);
extern size_t discord_gateway_close_opcodes_list_to_json_v(char *str, size_t len, void *p);
extern size_t discord_gateway_close_opcodes_list_to_json(char *str, size_t len, enum discord_gateway_close_opcodes **p);


enum discord_gateway_events {
  DISCORD_GATEWAY_EVENTS_NONE = 0,
  DISCORD_GATEWAY_EVENTS_READY = 1,
  DISCORD_GATEWAY_EVENTS_RESUMED = 2,
  DISCORD_GATEWAY_EVENTS_MESSAGE_CREATE = 37,
  DISCORD_GATEWAY_EVENTS_MESSAGE_UPDATE = 38,
  DISCORD_GATEWAY_EVENTS_MESSAGE_DELETE = 39,
};
struct discord;
struct discord_message;

typedef void (*discord_on_message)(struct discord *client, struct sized_buffer *);

struct discord_identify_connection {
  char *os; /**< your operating system */
  char *browser; /**< your library name */
  char *device; /**< your library name */
};

struct discord_activity {
  char *name;
  int type;
  char *url;
  u64_unix_ms_t created_at;
  u64_snowflake_t application_id;
  char *details;
  char *state;
  bool instance;
};

struct discord_presence_status {
  u64_unix_ms_t since; /**< unix time (in milliseconds) of when the client went idle, or null if the client is not idle */
  struct discord_activity **activities; /**< the user's activities */
  char *status; /**< the user's new status */
  bool afk; /**< whether or not the client is afk */
};

struct discord_identify {
  char *token;
  struct discord_identify_connection *properties;
  bool compress;
  int large_threshold;
  bool guild_subscriptions;
  struct discord_presence_status *presence;
  int intents;
};

struct discord_session_start_limit {
  int total; /**< the total number of session starts the current user is allowed */
  int remaining; /**< the remaining number of session starts the current user is allowed */
  int reset_after; /**< the number of milliseconds after which the limit resets */
  int max_concurrency; /**< the number of identify requests allowed per 5 seconds */
};

struct discord_attachment {
  char *content; /**< the file contents */
  int id; /**< attachment id */
  char *filename; /**< name of file attached */
  char *description; /**< description for the file */
  char *content_type; /**< the attachment's media type */
  size_t size; /**< size of file in bytes */
  char *url; /**< source url of file */
  char *proxy_url; /**< a proxied url of file */
  int height; /**< height of file (if image) */
  int width; /**< width of file (if image) */
  bool ephemeral; /**< whether this attachment is ephemeral */
};

enum discord_gateway_intents {
  DISCORD_GATEWAY_GUILDS = 1, /**< 1 << 0 */
  DISCORD_GATEWAY_GUILD_MEMBERS = 2, /**< 1 << 1 */
  DISCORD_GATEWAY_GUILD_BANS = 4, /**< 1 << 2 */
  DISCORD_GATEWAY_GUILD_EMOJIS = 8, /**< 1 << 3 */
  DISCORD_GATEWAY_GUILD_INTEGRATIONS = 16, /**< 1 << 4 */
  DISCORD_GATEWAY_GUILD_WEBHOOKS = 32, /**< 1 << 5 */
  DISCORD_GATEWAY_GUILD_INVITES = 64, /**< 1 << 6 */
  DISCORD_GATEWAY_GUILD_VOICE_STATES = 128, /**< 1 << 7 */
  DISCORD_GATEWAY_GUILD_PRESENCES = 256, /**< 1 << 8 */
  DISCORD_GATEWAY_GUILD_MESSAGES = 512, /**< 1 << 9 */
  DISCORD_GATEWAY_GUILD_MESSAGE_REACTIONS = 1024, /**< 1 << 10 */
  DISCORD_GATEWAY_GUILD_MESSAGE_TYPING = 2048, /**< 1 << 11 */
  DISCORD_GATEWAY_DIRECT_MESSAGES = 4096, /**< 1 << 12 */
  DISCORD_GATEWAY_DIRECT_MESSAGE_REACTIONS = 8192, /**< 1 << 13 */
  DISCORD_GATEWAY_DIRECT_MESSAGE_TYPING = 16384, /**< 1 << 14 */
};

#endif //DISCORD_DATATYPE_H
