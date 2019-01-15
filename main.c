#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/queue.h>

#define	RTE_DIM(a)	(sizeof (a) / sizeof ((a)[0]))
#define USE_LEN_MAX 32
#define HELP_ELN_MAX 1024


int main(int argc, char **argv)
{
    char **tokens = argv + 1;
    uint32_t n_tokens = argc -1;

    for(int i=0; i<n_tokens; i++){
        printf("%s\n", tokens[i]);
    }
    // printf("\n%d\n", RTE_DIM(argv));
    // printf("\n%d %d\n", argc, sizeof(argv));
    // printf("%s\n", argv[3]);
    // printf("\n%d\n", sizeof(argv[3][0]));
}

struct command {
    TAILQ_ENTRY(command) node;
    char use[USE_LEN_MAX];
    char* help;
    struct command* parent;
    struct command_list* subcommand;
};

TAILQ_HEAD(command_list, command);

int command_init(struct command* cmd) {
    cmd->parent = NULL;
    TAILQ_INIT(cmd->subcommand);
    return 0;
}


struct command* command_new(const char *use, const char* help) {
    struct command* cmd;

    if ((use == NULL) || (help == NULL)) {
        return NULL;
    }


    cmd = calloc(1, sizeof(struct command));
    if (cmd == NULL) {
        return NULL;
    }



    strncpy(cmd->use, use, sizeof(use));
    
    cmd->help = calloc(1, HELP_ELN_MAX+1);
    strncpy(cmd->help, help, HELP_ELN_MAX);

    cmd->parent = NULL;

    TAILQ_INIT(cmd->subcommand);

    return cmd;

}

struct command* command_list_find(struct command_list* list, const char *use){
    struct command*  cmd;
    TAILQ_FOREACH(cmd, list, node){
        if (strcmp(cmd->use, use) == 0){
            return cmd;
        }
    }
    return NULL;
}

void command_list_add(struct command_list* list, struct command* cmd){
    TAILQ_INSERT_TAIL(list, cmd, node);
}

void command_add_subcommand(struct command* parent, struct command* sub){
    sub->parent = parent;
    command_list_add(parent->subcommand, sub);
}

void command_find(char **tokens,
	uint32_t n_tokens){
        
    }