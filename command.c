#include "command.h"

struct command *
command_new(const char *name, const char *help, command_handle_t handle,
            ob *buf) {
  struct command *cmd;

  if ((name == NULL) || (help == NULL)) {
    return NULL;
  }

  cmd = calloc(1, sizeof(struct command));
  if (cmd == NULL) {
    return NULL;
  }

  cmd->name = calloc(1, NAME_LEN_MAX + 1);
  strncpy(cmd->name, name, NAME_LEN_MAX);

  cmd->help = calloc(1, HELP_LEN_MAX + 1);
  strncpy(cmd->help, help, HELP_LEN_MAX);

  cmd->handle = handle;
  cmd->output = buf;

  cmd->parent = NULL;
  cmd->subcommand = malloc(sizeof(struct command_list));
  TAILQ_INIT(cmd->subcommand);

  return cmd;
}

struct command *
command_list_find(struct command_list *list, const char *name) {
  struct command *cmd;
  TAILQ_FOREACH(cmd, list, node) {
    if (strcmp(cmd->name, name) == 0) {
      return cmd;
    }
  }
  return NULL;
}

void
command_add_subcommand(struct command *parent, struct command *sub) {
  if (sub == parent) {
    return;
  }
  sub->parent = parent;
  TAILQ_INSERT_TAIL(parent->subcommand, sub, node);
}

size_t
command_find(struct command *cmd, char **tokens, size_t n_tokens,
             struct command **target) {
  if (n_tokens == 0) {
    *target = cmd;
    return 0;
  }
  struct command *sub = command_list_find(cmd->subcommand, tokens[0]);
  if (sub == NULL) {
    *target = cmd;
    return 0;
  }

  return 1 + command_find(sub, tokens + 1, n_tokens - 1, target);
}

int
command_show_help(struct command *cmd) {
  ob_nprintf(cmd->output, "help info of '%s':\n", cmd->name);
  ob_nprintf(cmd->output, "usage:\t%s\n", cmd->help);
  if (!TAILQ_EMPTY(cmd->subcommand)) {
    ob_nprintf(cmd->output, "subcommand of '%s':\n", cmd->name);
    size_t max_use_len = 0;
    struct command *subcmd;
    TAILQ_FOREACH(subcmd, cmd->subcommand, node) {
      if (strlen(subcmd->name) > max_use_len) {
        max_use_len = strlen(subcmd->name);
      }
    }

    char expr[32];
    snprintf(expr, 32, "%%-%ds\t%%s\n", max_use_len);
    TAILQ_FOREACH(subcmd, cmd->subcommand, node) {
      ob_nprintf(cmd->output, expr, subcmd->name, subcmd->help);
    }
  }
};

int
command_run(struct command *cmd, char **tokens, size_t n_tokens) {
  struct command *target;
  size_t n = command_find(cmd, tokens, n_tokens, &target);
  if ((n_tokens > n) && (!strncmp("help", tokens[n], 4))) {
    command_show_help(target);
    return 0;
  }
  if (target->handle != NULL) {
    return target->handle(tokens + n, n_tokens - n);
  }

  if (!TAILQ_EMPTY(target->subcommand)) {
    ob_nprintf(cmd->output, "ERROR: no suitable subcommand for '");
    for (int i = 0; i < n; i++) {
      ob_nprintf(cmd->output, "%s ", tokens[i]);
    }
    ob_nprintf(cmd->output, "'\n");
  }

  ob_nprintf(cmd->output, "ERROR: no handler for '");
  for (int i = 0; i < n; i++) {
    ob_nprintf(cmd->output, "%s ", tokens[i]);
  }
  ob_nprintf(cmd->output, "'\n");

  command_show_help(target);

  return -1;
};
