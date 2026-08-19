#ifndef PARSER_H
#define PARSER_H

#include "token.h"

#define MAX_COMMANDS 20
#define MAX_ARGS 100

typedef struct
{
    char *argv[MAX_ARGS];
    int argc;

    char *input_file;
    char *output_file;

    int append;
    int background;

} Command;

typedef struct
{
    Command commands[MAX_COMMANDS];
    int command_count;

} Pipeline;

void parser_init(Pipeline *pipeline);

int parse_tokens(Token tokens[], Pipeline *pipeline);

void free_pipeline(Pipeline *pipeline);

#endif
