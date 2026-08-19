#ifndef EXPAND_H
#define EXPAND_H

#include "token.h"

#define MAX_EXPANDED_LENGTH 4096

int expand_variables(Token tokens[]);

#endif
