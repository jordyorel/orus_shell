#include "parser.h"
#include <ctype.h>


char **tokenize_input(const char *line, int *num_tokens) {
    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    int ntokens = 0;
    const char *p = line;

    while (*p) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;

        char buffer[MAX_COMMAND_LENGTH] = {0};
        int buf_index = 0;
        int expand = 1;

        if (*p == '"' || *p == '\'') {
            char quote = *p++;
            expand = (quote == '"');
            while (*p && *p != quote) {
                if (*p == '\\' && quote == '"') {
                    buffer[buf_index++] = *(++p);
                } else {
                    buffer[buf_index++] = *p;
                }
                p++;
            }
            if (*p == quote) p++;
        } else {
            while (*p && !isspace((unsigned char)*p)) {
                buffer[buf_index++] = *p++;
            }
        }
        buffer[buf_index] = '\0';

        // Variable expansion
        if (expand) {
            char expanded[MAX_COMMAND_LENGTH] = {0};
            int ei = 0;
            for (int i = 0; buffer[i]; i++) {
                if (buffer[i] == '$') {
                    char varname[128] = {0};
                    int vi = 0;
                    i++;

                    if (buffer[i] == '{') {
                        i++;
                        while (buffer[i] && buffer[i] != '}') {
                            varname[vi++] = buffer[i++];
                        }
                    } else {
                        while (buffer[i] &&
                               (isalnum((unsigned char)buffer[i]) ||
                                buffer[i] == '_')) {
                            varname[vi++] = buffer[i++];
                        }
                        i--;
                    }
                    varname[vi] = '\0';

                    char *value = getenv(varname);
                    if (value) {
                        strcpy(expanded + ei, value);
                        ei += strlen(value);
                    }
                } else {
                    expanded[ei++] = buffer[i];
                }
            }
            expanded[ei] = '\0';
            tokens[ntokens++] = strdup(expanded);
        } else {
            tokens[ntokens++] = strdup(buffer);
        }
    }
    tokens[ntokens] = NULL;
    *num_tokens = ntokens;
    return tokens;
}

char **expand_wildcards(char **tokens, int *num_tokens) {
    char **new_tokens = malloc(MAX_TOKENS * sizeof(char *));
    if (new_tokens == NULL) {
        perror("malloc failed");  // Handle malloc failure
        exit(1);                  // Or return an error code if you prefer
    }
    int ntokens = 0;

    for (int i = 0; tokens[i] != NULL; i++) {
        if (strpbrk(tokens[i], "*?") != NULL) {
            glob_t glob_results;
            int ret = glob(tokens[i], 0, NULL, &glob_results);
            if (ret == 0) {  // Successful glob
                if (glob_results.gl_pathc > 0) {
                    for (size_t j = 0; j < glob_results.gl_pathc; j++) {
                        new_tokens[ntokens++] =
                            strdup(glob_results.gl_pathv[j]);
                        if (new_tokens[ntokens - 1] == NULL) {
                            perror("strdup failed");
                            exit(1);
                        }
                    }
                } else {  // No match found: Keep the original token
                    new_tokens[ntokens++] = strdup(tokens[i]);
                    if (new_tokens[ntokens - 1] == NULL) {
                        perror("strdup failed");
                        exit(1);
                    }
                }
                globfree(&glob_results);
            } else {  // glob failed
                fprintf(stderr, COLOR_RED "glob failed: %s\n" COLOR_RESET,
                        strerror(ret));  // More informative error message
                new_tokens[ntokens++] = strdup(
                    tokens[i]);  // Keep original token even if glob fails
                if (new_tokens[ntokens - 1] == NULL) {
                    perror("strdup failed");
                    exit(1);
                }
            }
        } else {
            new_tokens[ntokens++] = strdup(tokens[i]);
            if (new_tokens[ntokens - 1] == NULL) {
                perror("strdup failed");
                exit(1);
            }
        }
    }
    new_tokens[ntokens] = NULL;
    *num_tokens = ntokens;

    // Free original tokens (after they've been copied)
    for (int i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);

    return new_tokens;
}

/* Command Parsing */
Command* parse_commands(char *input, int *num_commands) {
    Command *commands = malloc(MAX_PIPES * sizeof(Command));
    *num_commands = 0;

    char *saveptr1;
    char *cmd_str = strtok_r(input, "|", &saveptr1);

    while (cmd_str != NULL && *num_commands < MAX_PIPES) {
        // Trim whitespace
        while (*cmd_str == ' ') cmd_str++;
        size_t len = strlen(cmd_str);
        while (len > 0 && isspace((unsigned char)cmd_str[len - 1])) {
            cmd_str[len - 1] = '\0';
            len--;
        }

        // Check for empty command
        if (strlen(cmd_str) == 0) {
            fprintf(stderr,
                    COLOR_RED "Error: empty command in pipeline\n" COLOR_RESET);
            cmd_str = strtok_r(NULL, "|", &saveptr1);
            continue;
        }

        // Initialize redirection info
        commands[*num_commands].redirect = (RedirectInfo){0};
        commands[*num_commands].redirect.input_fd = -1;
        commands[*num_commands].redirect.output_fd = -1;
        commands[*num_commands].is_background = 0;

        // Tokenize and expand
        int tok_count = 0;
        char **raw_tokens = tokenize_input(cmd_str, &tok_count);
        raw_tokens = expand_wildcards(raw_tokens, &tok_count);

        // Process tokens for redirections and background
        char **tokens = malloc(MAX_TOKENS * sizeof(char *));
        int final_tok = 0;

        for (int i = 0; i < tok_count;) {
            if (strcmp(raw_tokens[i], "&") == 0) {
                commands[*num_commands].is_background = 1;
                free(raw_tokens[i++]);
                continue;
            }

            // Input redirection
            if (strcmp(raw_tokens[i], "<") == 0 && i + 1 < tok_count) {
                commands[*num_commands].redirect.input_file =
                    strdup(raw_tokens[i + 1]);
                free(raw_tokens[i]);
                free(raw_tokens[i + 1]);
                i += 2;
                continue;
            }

            // Output redirection
            if (strcmp(raw_tokens[i], ">") == 0 && i + 1 < tok_count) {
                commands[*num_commands].redirect.output_file =
                    strdup(raw_tokens[i + 1]);
                free(raw_tokens[i]);
                free(raw_tokens[i + 1]);
                i += 2;
                continue;
            }

            // Append redirection
            if (strcmp(raw_tokens[i], ">>") == 0 && i + 1 < tok_count) {
                commands[*num_commands].redirect.append_file =
                    strdup(raw_tokens[i + 1]);
                commands[*num_commands].redirect.is_append = 1;
                free(raw_tokens[i]);
                free(raw_tokens[i + 1]);
                i += 2;
                continue;
            }

            // Stderr redirection
            if (strcmp(raw_tokens[i], "2>&1") == 0) {
                commands[*num_commands].redirect.redirect_stderr = 1;
                free(raw_tokens[i++]);
                continue;
            }

            // Add to tokens
            tokens[final_tok++] = raw_tokens[i++];
        }
        tokens[final_tok] = NULL;
        free(raw_tokens);

        // Check if the command is empty after processing
        if (final_tok == 0) {
            fprintf(stderr, COLOR_RED "Error: empty command.\n" COLOR_RESET);
            free(tokens);
            cmd_str = strtok_r(NULL, "|", &saveptr1);
            continue;
        }

        commands[*num_commands].tokens = tokens;
        (*num_commands)++;

        cmd_str = strtok_r(NULL, "|", &saveptr1);
    }

    return commands;
}
