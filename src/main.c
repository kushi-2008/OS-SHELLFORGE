#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "expand.h"
#include "token.h"

static void print_tokens(Token tokens[])
{
    printf("\n------------ TOKENS ------------\n");

    for (int i = 0; i < MAX_TOKENS; i++)
    {
        if (tokens[i].type == TOKEN_EOF)
        {
            printf("%d : END         END\n", i);
            break;
        }

        printf("%d : %-11s %s\n",
               i,
               token_type_name(tokens[i].type),
               tokens[i].value);
    }

    printf("--------------------------------\n");
}

static void print_pipeline(Pipeline *pipeline)
{
    printf("\n========= PIPELINE ============\n");

    for (int i = 0; i < pipeline->command_count; i++)
    {
        Command *command = &pipeline->commands[i];

        printf("\nCommand %d\n", i + 1);
        printf("------------------------------\n");

        printf("Arguments\n");

        for (int j = 0; j < command->argc; j++)
        {
            printf("argv[%d] = %s\n",
                   j,
                   command->argv[j]);
        }

        if (command->input_file != NULL)
            printf("Input     : %s\n", command->input_file);
        else
            printf("Input     : None\n");

        if (command->output_file != NULL)
            printf("Output    : %s\n", command->output_file);
        else
            printf("Output    : None\n");

        printf("Append    : %s\n",
               command->append ? "Yes" : "No");

        printf("Background: %s\n",
               command->background ? "Yes" : "No");

        printf("==============================\n");
    }
}

int main(void)
{
    char input[MAX_INPUT_LENGTH];
    Token tokens[MAX_TOKENS];
    Pipeline pipeline;

    printf("================================\n");
    printf("        Shellforge\n");
    printf(" A Unix Style Shell written in C\n");
    printf("================================\n");

    while (1)
    {
        printf("shellforge$ ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0)
            continue;

        if (strcmp(input, "exit") == 0)
        {
            printf("Exiting...\n");
            break;
        }

        int token_count = tokenize(input, tokens);

        if (token_count <= 0)
        {
            printf("Lexer error\n");
            continue;
        }

        expand_variables(tokens);

        print_tokens(tokens);

        if (parse_tokens(tokens, &pipeline) != 0)
        {
            printf("Parser error\n");
            continue;
        }

        print_pipeline(&pipeline);

        free_pipeline(&pipeline);
    }

    return 0;
}

