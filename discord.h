/**
 * @file discord.h
 * @author cee-studio
 * @brief File containing public functions and datatypes
 *
 * These symbols are organized in a intuitive fashion to be easily
 *        matched to the official Discord API docs
 * @see https://discord.com/developers/docs/intro
 */

#ifndef DISCORD_H
#define DISCORD_H

#include <stdbool.h>
#include "json-actor-boxed.h"
#include "common.h"
#include "logconf.h"

/* see specs/discord/ for specs */
//#include "specs-code/discord/one-specs.h"

#define DISCORD_API_BASE_URL                 "https://discord.com/api/v9"
#define DISCORD_GATEWAY_URL_SUFFIX           "?v=9&encoding=json"
#define DISCORD_VOICE_CONNECTIONS_URL_SUFFIX "?v=4"

/* forward declarations */
struct discord;
struct discord_voice_cbs;
/**/

/** @defgroup DiscordLimitsSnowflake
 *  @{ */
#define DISCORD_SNOWFLAKE_INCREMENT          12
#define DISCORD_SNOWFLAKE_PROCESS_ID         17
#define DISCORD_SNOWFLAKE_INTERNAL_WORKER_ID 22
#define DISCORD_SNOWFLAKE_TIMESTAMP          64
/** @} */

/** @defgroup DiscordLimitsGeneral
 * @note assume the worst-case scenario for strings, where each character is 4
 *        bytes long (UTF8)
 *  @{ */
#define DISCORD_MAX_NAME_LEN          4 * 100 + 1
#define DISCORD_MAX_TOPIC_LEN         4 * 1024 + 1
#define DISCORD_MAX_DESCRIPTION_LEN   4 * 2048 + 1
#define DISCORD_MAX_USERNAME_LEN      4 * 32 + 1
#define DISCORD_MAX_DISCRIMINATOR_LEN 4 + 1
#define DISCORD_MAX_REASON_LEN        4 * 512 + 1
#define DISCORD_MAX_MESSAGE_LEN       4 * 2000 + 1
#define DISCORD_MAX_PAYLOAD_LEN       4 * 4096 + 1
#define DISCORD_MAX_VOICE_CONNECTIONS 512
/** @} */

/** @defgroup DiscordLimitsEmbed
 * @note assume the worst-case scenario for strings, where each character is 4
 *        bytes long (UTF8)
 *  @{ */
#define DISCORD_EMBED_TITLE_LEN       4 * 256 + 1
#define DISCORD_EMBED_DESCRIPTION_LEN 4 * 2048 + 1
#define DISCORD_EMBED_MAX_FIELDS      25
#define DISCORD_EMBED_FIELD_NAME_LEN  4 * 256 + 1
#define DISCORD_EMBED_FIELD_VALUE_LEN 4 * 1024 + 1
#define DISCORD_EMBED_FOOTER_TEXT_LEN 4 * 2048 + 1
#define DISCORD_EMBED_AUTHOR_NAME_LEN 4 * 256 + 1
/** @} */

/** @defgroup DiscordLimitsWebhook
 * @note assume the worst-case scenario for strings, where each character is 4
 *        bytes long (UTF8)
 *  @{ */
#define DISCORD_WEBHOOK_NAME_LEN 4 * 80 + 1
/** @} */

/** @defgroup OrcaDiscordCodes
 * @see @ref OrcaCodes for non-Discord errors
 *  @{ */
/** Received a JSON error message */
#define ORCA_DISCORD_JSON_CODE 1
/** Bad authentication token */
#define ORCA_DISCORD_BAD_AUTH 2
/** Being ratelimited */
#define ORCA_DISCORD_RATELIMIT 3
/** Couldn't establish connection to Discord */
#define ORCA_DISCORD_CONNECTION 4
/** @} OrcaDiscordCodes */

/******************************************************************************
 * Functions specific to the Discord client
 ******************************************************************************/

/**
 * @brief Return the meaning of ORCAcode
 * @note in case of a @ref ORCA_DISCORD_JSON_CODE and if `client` is provided,
 *        a much more descriptive JSON message will be returned instead.
 *
 * @param code the ORCAcode to be explained
 * @param client the client created with discord_init(), NULL for generic error
 *        descriptions
 * @return a string containing the code meaning
 */
const char *discord_strerror(ORCAcode code, struct discord *client);

/**
 * @brief Create a Discord Client handle by its token
 * @see discord_get_logconf() to configure logging behavior
 *
 * @param token the bot token
 * @return the newly created Discord Client handle
 */
struct discord *discord_init(const char token[]);

/**
 * @brief Create a Discord Client handle by a bot.config file
 *
 * @param config_file the bot.config file name
 * @return the newly created Discord Client handle
 */
struct discord *discord_config_init(const char config_file[]);
struct discord *discord_sb_init(char *buf, size_t buf_size);

/**
 * @brief Clone a discord client
 *
 * Should be called before entering a thread, to ensure each thread
 *        has its own client instance with unique buffers, url and headers
 * @param orig_client the original client created with discord_init()
 * @return the original client clone
 */
struct discord *discord_clone(const struct discord *orig_client);

/**
 * @brief Free a Discord Client handle
 *
 * @param client the client created with discord_init()
 */
void discord_cleanup(struct discord *client);

/**
 * @deprecated use orca_global_init() instead
 * @brief Initialize resources of globals used by discord.h
 */
void discord_global_init();

/**
 * @deprecated use orca_global_cleanup() instead
 * @brief Free resources of globals used by discord.h
 */
void discord_global_cleanup();

/**
 * @brief Get the client's cached user
 *
 * @param client the client created with discord_init()
 * @warning the returned structure should NOT be modified
 */
const struct discord_user *discord_get_self(struct discord *client);

/**
 * @brief Subscribe to Discord Events
 *
 * @param client the client created with discord_init()
 * @param code the intents opcode, can be set as a bitmask operation
 */

void discord_set_prefix(struct discord *client, char *prefix);

/**
 * @brief Start a connection to the Discord Gateway
 *
 * @param client the client created with discord_init()
 * @ORCA_return
 */
ORCAcode discord_run(struct discord *client);

/**
 * @brief Gracefully shutdown an ongoing Discord connection
 *
 * @param client the client created with discord_init()
 */
void discord_shutdown(struct discord *client);

/**
 * @brief Gracefully reconnects an ongoing Discord connection
 *
 * @param client the client created with discord_init()
 * @param resume true to attempt to resume to previous session,
 *        false restart a fresh session
 */
void discord_reconnect(struct discord *client, bool resume);

/**
 * @brief Store user arbitrary data that can be retrieved by discord_get_data()
 *
 * @param client the client created with discord_init()
 * @param data user arbitrary data
 * @return pointer to user data
 * @warning the user should provide his own locking mechanism to protect
 *        his data from race conditions
 */
void *discord_set_data(struct discord *client, void *data);

/**
 * @brief Receive user arbitrary data stored with discord_set_data()
 *
 * @param client the client created with discord_init()
 * @return pointer to user data
 * @warning the user should provide his own locking mechanism to protect
 *        his data from race conditions
 */
void *discord_get_data(struct discord *client);

/**
 * @brief Get the client WebSockets ping
 * @note Only works after a connection has been established via discord_run()
 *
 * @param client the client created with discord_init()
 * @return the ping in milliseconds
 */
int discord_get_ping(struct discord *client);

/**
 * @brief Get the current timestamp (in milliseconds)
 *
 * @param client the client created with discord_init()
 * @return the timestamp in milliseconds
 */
uint64_t discord_timestamp(struct discord *client);

/**
 * @brief Retrieve client's logging module for configuration purposes
 * @see logconf_setup(), logconf_set_quiet(), logconf_set_level()
 *
 * @param client the client created with discord_init()
 * @return the client's logging module
 */
struct logconf *discord_get_logconf(struct discord *client);

/******************************************************************************
 * Functions specific to Discord's REST API
 ******************************************************************************/

/** @brief Async `done` callback return context */
struct discord_async_ret {
  /**
   * the request's response object (`NULL` if missing)
   * @note can be safely cast to the request's return type
   */
  const void *ret;
  /** user arbitrary data (`NULL` if missing)*/
  void *data;
};

/** @brief Triggers on a successful async request */
typedef void (*discord_on_done)(struct discord *client,
                                struct discord_async_ret *ret);

/** @brief Async `fail` callback return context */
struct discord_async_err {
  /** request error code @see discord_strerror() */
  ORCAcode code;
  /** user arbitrary data (`NULL` if missing)*/
  void *data;
};

/** @brief Triggers on a failed async request */
typedef void (*discord_on_fail)(struct discord *client,
                                struct discord_async_err *err);

/** @brief The async attributes for next request */
struct discord_async_attr {
  /** optional callback to be executed on a succesful request */
  discord_on_done done;
  /** optional callback to be executed on a failed request */
  discord_on_fail fail;
  /** whether the next request is high priority (enqueued first) */
  bool high_p;
  /** optional user data to be sent over */
  void *data;
  /** optional user data cleanup function */
  void (*cleanup)(void *data);
};


/**
 * @brief Get a single valid WSS URL, which the client can use for connecting
 * @note This route should be cached, and only call the function again if
 *        unable to properly establishing a connection with the cached version
 *
 * @param client the client created with discord_init()
 * @param ret if successful, a @ref sized_buffer containing the JSON response
 * @ORCA_return
 */
ORCAcode discord_get_gateway(struct discord *client, struct sized_buffer *ret);

/**
 * @brief Get a single valid WSS URL, and additional metadata that can help
 *        during the operation of large bots.
 * @note This route should not be cached for extended periods of time as the
 *        value is not guaranteed to be the same per-call, and changes as the
 *        bot joins/leaves guilds
 *
 * @param client the client created with discord_init()
 * @param ret if successful, a @ref sized_buffer containing the JSON response
 * @ORCA_return
 */
ORCAcode discord_get_gateway_bot(struct discord *client,
                                 struct sized_buffer *ret);


/******************************************************************************
 * Functions specific to Discord's Gateway
 ******************************************************************************/

/**
 * @brief return value of discord_set_event_scheduler() callback
 * @see discord_set_event_scheduler()
 */
typedef enum discord_event_scheduler {
  /** this event has been handled */
  DISCORD_EVENT_IGNORE,
  /** handle this event in main thread */
  DISCORD_EVENT_MAIN_THREAD,
  /** handle this event in a worker thread */
  DISCORD_EVENT_WORKER_THREAD
} discord_event_scheduler_t;

/**
 * @brief Event Handling Mode callback
 *
 * A very important callback that enables the user with a fine-grained control
 *        of how each event is handled: blocking, non-blocking or ignored
 * @see discord_set_event_scheduler(), @ref discord_gateway_events
 */
typedef enum discord_event_scheduler (*discord_on_scheduler)(
  struct discord *client,
  struct sized_buffer *event_data,
  enum discord_gateway_events event);

/**
 * @brief Provides the user with a fine-grained control of the Discord's
 *       event-loop
 *
 * Allows the user to specify which events should be executed from the
 *       main-thread, in parallel from a worker-thread, or completely ignored
 *
 * @param client the client created_with discord_init()
 * @param fn the function that will be executed
 * @warning The user is responsible for providing his own locking mechanism to
 *       avoid race-condition on sensitive data
 * @see @ref discord_event_scheduler, @ref discord_gateway_events
 */
void discord_set_event_scheduler(struct discord *client,
                                 discord_on_scheduler callback);

typedef void (*discord_on_idle)(struct discord *client);

void discord_set_on_ready(struct discord *client, discord_on_idle cb);
void discord_set_on_message_create(struct discord *client,
                                   discord_on_message cb);
void discord_set_on_message_update(struct discord *client,
                                   discord_on_message cb);
void discord_set_on_message_delete(struct discord *client,
                                   discord_on_message cb);
#endif /* DISCORD_H */
