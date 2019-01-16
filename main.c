#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/queue.h>

#define RTE_DIM(a) (sizeof(a) / sizeof((a)[0]))
#define USE_LEN_MAX 32
#define HELP_ELN_MAX 1024

typedef void (*command_handle_t)(char **tokens,
                                 uint32_t n_tokens);



struct command
{
    TAILQ_ENTRY(command)
    node;
    char use[USE_LEN_MAX];
    char *help;
    command_handle_t handle;
    struct command *parent;
    struct command_list *subcommand;
};

TAILQ_HEAD(command_list, command);

int command_init(struct command *cmd)
{
    cmd->parent = NULL;
    TAILQ_INIT(cmd->subcommand);
    return 0;
}

struct command *command_new(const char *use, const char *help)
{
    struct command *cmd;

    if ((use == NULL) || (help == NULL))
    {
        return NULL;
    }

    printf("E1");
    cmd = calloc(1, sizeof(struct command));
    if (cmd == NULL)
    {
        return NULL;
    }

    strncpy(cmd->use, use, sizeof(use));

    cmd->help = calloc(1, HELP_ELN_MAX + 1);
    strncpy(cmd->help, help, HELP_ELN_MAX);

    cmd->parent = NULL;
    // cmd->subcommand = &TAILQ_HEAD_INITIALIZER(command_list);
    printf("E1");

    TAILQ_INIT(cmd->subcommand);
    printf("E1");

    return cmd;
    return NULL;
}

struct command *command_list_find(struct command_list *list, const char *use)
{
    struct command *cmd;
    TAILQ_FOREACH(cmd, list, node)
    {
        if (strcmp(cmd->use, use) == 0)
        {
            return cmd;
        }
    }
    return NULL;
}

void command_list_add(struct command_list *list, struct command *cmd)
{
    TAILQ_INSERT_TAIL(list, cmd, node);
}

void command_add_subcommand(struct command *parent, struct command *sub)
{
    sub->parent = parent;
    command_list_add(parent->subcommand, sub);
}

void command_find(struct command *cmd, char **tokens,
                  uint32_t n_tokens)
{
    if (n_tokens == 0)
    {
        return;
    }
    struct command *target;
    target = command_list_find(cmd->subcommand, tokens[0]);
    if (target == NULL)
    {
        return;
    }
    command_find(target, tokens + 1, n_tokens - 1);
}

void command_exec(struct command *cmd, char **tokens, u_int32_t n_tokens)
{
    if (cmd->handle != NULL)
    {
        cmd->handle(tokens, n_tokens);
        return;
    }
    if (cmd->subcommand != NULL)
    {
        // show subcommnad
        return;
    }
    // show error
}

void command_show_subcommand(struct command *cmd, char *out,
                             size_t out_size)
{
    if (cmd->subcommand == NULL)
    {
        snprintf(out, out_size, "");
        return;
    }

    size_t max_use_len;
    struct command *subcmd;
    TAILQ_FOREACH(subcmd, cmd->subcommand, node){
        if (strlen(subcmd->use) > max_use_len){
            max_use_len = strlen(subcmd->use);
        }
    }

    char expr[16];
    snprintf(expr, 16, "%%-%ds\t%%s\n", max_use_len);

    char *target = out;
    TAILQ_FOREACH(subcmd, cmd->subcommand, node){
        size_t unused_size = out_size - strlen(out);
        if (0 == unused_size) {
            break;
        }
        target += snprintf(target, unused_size, expr, cmd->use, cmd->help);
    }
}


int main(int argc, char **argv)
{
    char **tokens = argv + 1;
    uint32_t n_tokens = argc - 1;

    for (int i = 0; i < n_tokens; i++)
    {
        printf("%s\n", tokens[i]);
    }
    char expr[16];
    snprintf(expr, 10, "%%-%ds\t%%s\n", 3);
    printf("%s", expr);
    printf(expr, "aaa", "this is aaa");
    printf("%d\n", strlen(expr));
    // printf("\n%d\n", RTE_DIM(argv));
    // printf("\n%d %d\n", argc, sizeof(argv));
    // printf("%s\n", argv[3]);
    // printf("\n%d\n", sizeof(argv[3][0]));

    struct command* root_cmd=command_new("root", "root cmd");
    // struct command* cmd1=command_new("cmd1", "this is cmd 1");
    // struct command* cmd2=command_new("cmd2", "this is cmd 2");
    // command_add_subcommand(root_cmd, cmd1);
    // command_add_subcommand(root_cmd, cmd2);

    // char output[1000];
    // command_show_subcommand(root_cmd, output, 1000);
}