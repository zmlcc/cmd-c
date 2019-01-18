#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/queue.h>

#include "ob.h"

#define RTE_DIM(a) (sizeof(a) / sizeof((a)[0]))
#define NAME_LEN_MAX 32
#define HELP_LEN_MAX 1024

typedef int (*command_handle_t)(char **tokens,
                                uint32_t n_tokens);

TAILQ_HEAD(command_list, command);

struct command
{
    char *name;
    char *help;
    command_handle_t handle;
    ob *output;
    struct command *parent;
    struct command_list *subcommand;
    TAILQ_ENTRY(command)
    node;
};

static inline int command_init(struct command *cmd)
{
    cmd->parent = NULL;
    cmd->subcommand = malloc(sizeof(struct command_list));
    TAILQ_INIT(cmd->subcommand);
    return 0;
}

struct command *command_new(const char *name, const char *help, command_handle_t handle, ob *buf)
{
    struct command *cmd;

    if ((name == NULL) || (help == NULL))
    {
        return NULL;
    }

    cmd = calloc(1, sizeof(struct command));
    if (cmd == NULL)
    {
        return NULL;
    }

    cmd->name = calloc(1, NAME_LEN_MAX + 1);
    strncpy(cmd->name, name, NAME_LEN_MAX);

    cmd->help = calloc(1, HELP_LEN_MAX + 1);
    strncpy(cmd->help, help, HELP_LEN_MAX);

    cmd->handle = handle;
    cmd->output = buf;

    command_init(cmd);

    printf("Name: %s, Function: %d", cmd->name, cmd->handle == NULL);

    return cmd;
}

struct command *command_list_find(struct command_list *list, const char *name)
{
    struct command *cmd;
    TAILQ_FOREACH(cmd, list, node)
    {
        if (strcmp(cmd->name, name) == 0)
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

// struct command* command_find(struct command *cmd, char **tokens,
//                   uint32_t n_tokens, uint32_t* p_token)
// {
//     if (n_tokens == *p_token)
//     {
//         return cmd;
//     }
//     struct command *target;
//     target = command_list_find(cmd->subcommand, tokens+(*p_token));
//     if (target == NULL)
//     {
//         return cmd;
//     }
//     (*p_token)++;
//     return command_find(target, tokens, n_tokens, p_token) ;
// }

uint32_t command_find(struct command *cmd, char **tokens,
                      uint32_t n_tokens, struct command **target)
{
    if (n_tokens == 0)
    {
        *target = cmd;
        return 0;
    }
    struct command* sub = command_list_find(cmd->subcommand, tokens[0]);
    if (sub == NULL)
    {
    printf("FUCK here");
        *target = cmd;
        printf("FUCK TT %d %d\n", cmd, *target);
        return 0;
    }

    return 1 + command_find(sub, tokens + 1, n_tokens - 1, target);
}

void command_exec(struct command *cmd, char **tokens, u_int32_t n_tokens)
{
    if (cmd->handle != NULL)
    {
        cmd->handle(tokens, n_tokens);
        return;
    }
    if
        TAILQ_EMPTY(cmd->subcommand)
        {
            // show subcommnad
            return;
        }
    ob_nprintf(cmd->output, "ERROR: no handler for '%s'", "aaa");
}

void command_show_subcommand(struct command *cmd, char *out,
                             size_t out_size)
{
    if
        TAILQ_EMPTY(cmd->subcommand)
        {
            snprintf(out, out_size, "");
            return;
        }

    size_t max_use_len = 0;
    struct command *subcmd;
    TAILQ_FOREACH(subcmd, cmd->subcommand, node)
    {
        if (strlen(subcmd->name) > max_use_len)
        {
            max_use_len = strlen(subcmd->name);
        }
    }

    char expr[32];
    snprintf(expr, 32, "%%-%ds\t%%s\n", max_use_len);

    char *target = out;
    TAILQ_FOREACH(subcmd, cmd->subcommand, node)
    {
        size_t unused_size = out + out_size - target;
        int used_size = snprintf(target, unused_size, expr, subcmd->name, subcmd->help);
        if (used_size < 0 || used_size >= unused_size)
        {
            break;
        }
        target += used_size;
    }
}

int cmd_run(struct command *cmd, char **tokens, u_int32_t n_tokens)
{
    struct command *target;
    uint32_t n = command_find(cmd, tokens, n_tokens, &target);
    printf("FUCK TT %d %d\n", cmd, target);
    printf("FUCKN %d\n", n);
    printf("WHAT %d\n", target == cmd);
    if (target->handle != NULL)
    {
         printf("Name: %s, Function: %d", target->name, target->handle == NULL);
        printf("FUCK5\n");
        return cmd->handle(tokens, n_tokens);
    }
    printf("FUCK6\n");
    if
        TAILQ_EMPTY(target->subcommand)
        {
            printf("FUCK2\n");
            ob_nprintf(cmd->output, "ERROR: no handler for '");
            for (int i = 0; i < n; i++)
            {
                ob_nprintf(cmd->output, "%s ", tokens[i]);
            }
            ob_nprintf(cmd->output, "'\n");
            return -1;
        }
            printf("FUCK3\n");

    ob_nprintf(cmd->output, "ERROR: no suitable subcommand for '");
    for (int i = 0; i < n; i++)
    {
        ob_nprintf(cmd->output, "%s ", tokens[i]);
    }
    ob_nprintf(cmd->output, "'\n");
    size_t max_use_len = 0;
    struct command *subcmd;
    TAILQ_FOREACH(subcmd, target->subcommand, node)
    {
        if (strlen(subcmd->name) > max_use_len)
        {
            max_use_len = strlen(subcmd->name);
        }
    }

    char expr[32];
    snprintf(expr, 32, "%%-%ds\t%%s\n", max_use_len);
    TAILQ_FOREACH(subcmd, cmd->subcommand, node)
    {
        ob_nprintf(cmd->output, expr, subcmd->name, subcmd->help);
    }
    return -1;
};

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

    ob* buf = ob_new(8192);

    struct command *root_cmd = command_new("root", "this is root cmd", NULL, buf);
    struct command *cmd1 = command_new("ffdsa", "fdsa", NULL, buf);
    struct command *cmd2 = command_new("cmd2verylong", "this is cmd 2", NULL, buf);
    char output[100];
    command_add_subcommand(root_cmd, cmd1);
    printf("\n");
    // command_show_subcommand(root_cmd, output, 10);
    // printf("%s\n", output);

    command_add_subcommand(root_cmd, cmd2);
    printf("\n");
    // command_show_subcommand(root_cmd, output, 20);
    // printf("%s\n", output);
    cmd_run(root_cmd, tokens, n_tokens);
    printf("%s\n", buf->buffer);
}