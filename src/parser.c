#include <stdlib.h>
#include <string.h>

#include "parser.h"

void parser_init(Pipeline *pipeline)
{
    if (pipeline == NULL)
        return;

    pipeline->command_count = 0;

    for (int i = 0; i < MAX_COMMANDS; i++)
    {
        pipeline->commands[i].argc = 0;
        pipeline->commands[i].input_file = NULL;
        pipeline->commands[i].output_file = NULL;
        pipeline->commands[i].append = 0;
        pipeline->commands[i].background = 0;

        for (int j = 0; j < MAX_ARGS; j++)
            pipeline->commands[i].argv[j] = NULL;
    }
}

static char *copy_string(const char *text)
{
    if (text == NULL)
        return NULL;

    char *copy = malloc(strlen(text) + 1);

    if (copy == NULL)
        return NULL;

    strcpy(copy, text);
    return copy;
}

int parse_tokens(Token tokens[], Pipeline *pipeline)
{
    if (tokens == NULL || pipeline == NULL)
        return -1;

    parser_init(pipeline);

    int command_index = 0;
    int i = 0;

    pipeline->command_count = 1;

    while (tokens[i].type != TOKEN_EOF)
    {
        TokenType type = tokens[i].type;

        /* Pipe */
        if (type == TOKEN_PIPE)
        {
            if (command_index + 1 >= MAX_COMMANDS)
                return -1;

            command_index++;
            pipeline->command_count++;

            i++;
            continue;
        }

        /* Input redirection */
        if (type == TOKEN_REDIRECT_IN)
        {
            i++;

            if (tokens[i].type == TOKEN_EOF)
                return -1;

            pipeline->commands[command_index].input_file =
                copy_string(tokens[i].value);

            i++;
            continue;
        }

        /* Output redirection */
        if (type == TOKEN_REDIRECT_OUT)
        {
            i++;

            if (tokens[i].type == TOKEN_EOF)
                return -1;

            pipeline->commands[command_index].output_file =
                copy_string(tokens[i].value);

            pipeline->commands[command_index].append = 0;

            i++;
            continue;
        }

        /* Append redirection */
        if (type == TOKEN_REDIRECT_APPEND)
        {
            i++;

            if (tokens[i].type == TOKEN_EOF)
                return -1;

            pipeline->commands[command_index].output_file =
                copy_string(tokens[i].value);

            pipeline->commands[command_index].append = 1;

            i++;
            continue;
        }

        /* Background */
        if (type == TOKEN_OPERATOR &&
            strcmp(tokens[i].value, "&") == 0)
        {
            pipeline->commands[command_index].background = 1;

            i++;
            continue;
        }

        /* Normal argument */
        if (pipeline->commands[command_index].argc < MAX_ARGS - 1)
        {
            pipeline->commands[command_index].argv[
                pipeline->commands[command_index].argc
            ] = copy_string(tokens[i].value);

            pipeline->commands[command_index].argc++;
        }

        i++;
    }

    /*
     * NULL terminate argv
     */
    for (int j = 0; j < pipeline->command_count; j++)
    {
        pipeline->commands[j].argv[
            pipeline->commands[j].argc
        ] = NULL;
    }

    return 0;
}

void free_pipeline(Pipeline *pipeline)
{
    if (pipeline == NULL)
        return;

    for (int i = 0; i < pipeline->command_count; i++)
    {
        Command *command = &pipeline->commands[i];

        for (int j = 0; j < command->argc; j++)
        {
            free(command->argv[j]);
            command->argv[j] = NULL;
        }

        free(command->input_file);
        free(command->output_file);

        command->input_file = NULL;
        command->output_file = NULL;
    }

    pipeline->command_count = 0;
}
