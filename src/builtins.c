#include "builtins.h"
#include "jobs.h"

void handle_builtin_command(char **tokens) {
    if (strcmp(tokens[0], "exit") == 0) {
        printf(COLOR_BLUE "Exiting shell..\n" COLOR_RESET);
        exit(0);
    } else if (strcmp(tokens[0], "cd") == 0) {
        if (tokens[1] == NULL) {
            fprintf(stderr,
                    COLOR_RED "cd: Destination not specified.\n" COLOR_RESET);
            return;
        }
        if (chdir(tokens[1]) != 0) {
            perror("cd");
        }
    } else if (strcmp(tokens[0], "clear") == 0) {
        system("clear");
    } else if (strcmp(tokens[0], "jobs") == 0) {
        list_jobs();
    } else if (strcmp(tokens[0], "export") == 0) {
        if (tokens[1] == NULL) {
            fprintf(stderr,
                    COLOR_RED "export: Variable not specified.\n" COLOR_RESET);
            return;
        }
        char *envstr = strdup(tokens[1]);
        if (envstr == NULL) {
            perror("strdup");
            return;
        }
        if (putenv(envstr) != 0) {
            perror("export");
            free(envstr);  // Free envstr on error
            return;
        }
    }
}
