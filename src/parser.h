#ifndef PARSER_H
#define PARSER_H

#include "orus.h"

char **tokenize_input(const char *line, int *num_tokens);
Command *parse_commands(char *input, int *num_commands);

#endif
