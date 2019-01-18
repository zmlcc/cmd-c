#include "command.h"
#include <stdio.h>

int
root_handle(char **tokens, size_t n_tokens) {
  printf("this is root command, get tokens: ");
  for (int i = 0; i < n_tokens; i++) {
    printf("%s ", tokens[i]);
  }
}

int
aaa_handle(char **tokens, size_t n_tokens) {
  printf("this is aaa command, get tokens: ");
  for (int i = 0; i < n_tokens; i++) {
    printf("%s ", tokens[i]);
  }
}

int
main(int argc, char **argv) {
  char **tokens = argv + 1;
  size_t n_tokens = argc - 1;

  for (int i = 0; i < n_tokens; i++) {
    printf("%s\n", tokens[i]);
  }

  ob *buf = ob_new(8192);

  struct command *root_cmd =
      command_new("root", "this is root cmd", root_handle, buf);
  struct command *cmd1 = command_new("aaa", "aaa test ", aaa_handle, buf);
  struct command *cmd2 =
      command_new("cmd2verylong", "this is cmd 2", NULL, buf);

  command_add_subcommand(root_cmd, cmd1);

  command_add_subcommand(root_cmd, cmd2);

  command_run(root_cmd, tokens, n_tokens);
  printf("%s\n", buf->buffer);
}