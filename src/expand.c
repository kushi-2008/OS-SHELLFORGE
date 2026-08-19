#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "expand.h"
#include "token.h"

#ifndef MAX_TOKENS
#define MAX_TOKENS 1024
#endif

#ifndef MAX_TOKEN_LENGTH
#define MAX_TOKEN_LENGTH 1024
#endif

static int expand_string(const char *input, char *output, size_t output_size)
{
    size_t i = 0;
    size_t j = 0;

    if (input == NULL || output == NULL || output_size == 0)
        return -1;

    while (input[i] != '\0' && j < output_size - 1)
    {
        if (input[i] != '$')
        {
            output[j++] = input[i++];
            continue;
        }

        /* '$' at the end */
        if (input[i + 1] == '\0')
        {
            output[j++] = '$';
            i++;
            continue;
        }

        char variable[MAX_TOKEN_LENGTH];
        size_t var_len = 0;

        /* ${VARIABLE} */
        if (input[i + 1] == '{')
        {
            size_t k = i + 2;

            while (input[k] != '\0' &&
                   input[k] != '}' &&
                   var_len < MAX_TOKEN_LENGTH - 1)
            {
                variable[var_len++] = input[k++];
            }

            /* No closing brace */
            if (input[k] != '}')
            {
                output[j++] = '$';
                i++;
                continue;
            }

            variable[var_len] = '\0';

            const char *value = getenv(variable);

            if (value != NULL)
            {
                while (*value != '\0' && j < output_size - 1)
                    output[j++] = *value++;
            }

            i = k + 1;
        }
        else
        {
            /* $VARIABLE */
            size_t k = i + 1;

            while (input[k] != '\0' &&
                   (isalnum((unsigned char)input[k]) ||
                    input[k] == '_') &&
                   var_len < MAX_TOKEN_LENGTH - 1)
            {
                variable[var_len++] = input[k++];
            }

            /* Not a variable */
            if (k == i + 1)
            {
                output[j++] = '$';
                i++;
                continue;
            }

            variable[var_len] = '\0';

            const char *value = getenv(variable);

            if (value != NULL)
            {
                while (*value != '\0' && j < output_size - 1)
                    output[j++] = *value++;
            }

            i = k;
        }
    }

    output[j] = '\0';

    return 0;
}


int expand_variables(Token tokens[])
{
    if (tokens == NULL)
        return -1;

    for (int i = 0; i < MAX_TOKENS; i++)
    {
        /*
         * Token.value is an array, so we test its first character.
         */
        if (tokens[i].value[0] == '\0')
            break;

        char expanded[MAX_TOKEN_LENGTH];

        if (expand_string(
                tokens[i].value,
                expanded,
                sizeof(expanded)) != 0)
        {
            return -1;
        }

        /*
         * value is a fixed-size array, so copy into it.
         */
        strncpy(
            tokens[i].value,
            expanded,
            MAX_TOKEN_LENGTH - 1
        );

        tokens[i].value[MAX_TOKEN_LENGTH - 1] = '\0';
    }

    return 0;
}
