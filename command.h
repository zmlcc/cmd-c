#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

#include "ob.h"

#define NAME_LEN_MAX 32
#define HELP_LEN_MAX 1024

typedef int (*command_handle_t)(char **tokens, size_t n_tokens);

TAILQ_HEAD(command_list, command);

struct command {
  char *name;
  char *help;
  command_handle_t handle;
  ob *output;
  struct command *parent;
  struct command_list *subcommand;
  TAILQ_ENTRY(command) node;
};

struct command *
command_new(const char *name, const char *help, command_handle_t handle,
            ob *buf);

struct command *
command_list_find(struct command_list *list, const char *name);

void
command_add_subcommand(struct command *parent, struct command *sub);

size_t
command_find(struct command *cmd, char **tokens, size_t n_tokens,
             struct command **target);

int
command_show_help(struct command *cmd);

int
command_run(struct command *cmd, char **tokens, size_t n_tokens);