#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* strcmp() */
#include <assert.h>
#include <pthread.h>

#include "websockets.h"
#include "discord-datatype.h"
#include "discord.h"
#include "cee-utils.h"
#include "json-actor.h"
#include "discord-internal.h"

#define THREADPOOL_SIZE "4"


static void on_ready(struct discord *client) {
  logconf_error(&client->gw.conf, "Successfully connected to Discord!");
}

static void on_message_create(struct discord *client, struct sized_buffer *sb){
  logconf_error(&client->gw.conf, "%.*s", sb->size, sb->start);
  uint64_t channel_id, message_id;
  bool is_bot = false;
  char message[1024*4] = {0};
  json_extract(sb->start, sb->size,
               "(content):s, (id):F, (channel_id):F, (author.bot):b",
               message,
               cee_strtoull, &message_id,
               cee_strtoull, &channel_id,
               &is_bot);

  logconf_error(&client->gw.conf, "is_bot %d, send to channel id %s, %lld",
                is_bot, message, channel_id);

  if( is_bot ) return;
}

static void on_message_update(struct discord *client, struct sized_buffer *msg) {
  logconf_error(&client->gw.conf, "%.*s", msg->size, msg->start);
}

static void on_message_delete(struct discord *client, struct sized_buffer *msg) {
  logconf_error(&client->gw.conf, "%.*s", msg->size, msg->start);
}

static enum discord_event_scheduler
scheduler(struct discord *client, struct sized_buffer *data,
          enum discord_gateway_events event) {
  logconf_error(&client->gw.conf, "scheduler is called %d", event);
  return DISCORD_EVENT_WORKER_THREAD;
}

int main(int argc, char *argv[])
{
  const char *config_file;
  if (argc > 1)
    config_file = argv[1];
  else
    config_file = "bot.config";

  orca_global_init();
  //discord_global_init();

  struct discord *client = discord_config_init(config_file);
  assert(NULL != client && "Couldn't initialize client");

  /* trigger event callbacks in a multi-threaded fashion */
  discord_set_event_scheduler(client, &scheduler);
  
  discord_set_on_ready(client, &on_ready);
  discord_set_on_message_create(client, &on_message_create);
  discord_set_on_message_update(client, on_message_update);
  discord_set_on_message_delete(client, on_message_delete);

  printf("\n\nThis bot demonstrates how easy it is to setup a bot that"
         " echoes user actions.\n"
         "1. Send a message in any chat\n"
         "2. Edit that message\n"
         "3. Delete that message\n"
         "4. Add a reaction to a message\n"
         "5. Have another bot bulk-delete messages\n"
         "\nTYPE ANY KEY TO START BOT\n");
  fgetc(stdin); // wait for input

  discord_run(client);

  discord_cleanup(client);

  //discord_global_cleanup();
  orca_global_cleanup();
}



#if 0
int bridge_main_sb(char *buf, size_t buf_size){
  setenv("ORCA_THREADPOOL_SIZE", THREADPOOL_SIZE, 1);
  setenv("ORCA_THREADPOOL_QUEUE_SIZE", "128", 1);

  orca_global_init();
  struct discord *client = discord_sb_init(buf, buf_size);

  /* trigger event callbacks in a multi-threaded fashion */
  discord_set_event_scheduler(client, &scheduler);

  discord_set_on_ready(client, &on_ready);
  discord_set_on_message_create(client, on_message_create);
  discord_set_on_message_update(client, on_message_update);
  discord_set_on_message_delete(client, on_message_delete);

  discord_run(client);
  discord_cleanup(client);
  orca_global_cleanup();
  return 0;
}
#endif

