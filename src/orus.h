#ifndef ORUS_H
#define ORUS_H

#include <fcntl.h>
#include <glob.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "util.h"

/* Constants and Color Definitions */
#define MAX_COMMAND_LENGTH 1024
#define MAX_TOKENS 128
#define MAX_PIPES 10
#define MAX_JOBS 100

#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_BLUE "\033[34m"
#define COLOR_RESET "\033[0m"

typedef struct {
    char *input_file;
    char *output_file;
    char *append_file;
    int input_fd;
    int output_fd;
    int is_append;
    int redirect_stderr;
} RedirectInfo;

typedef struct {
    char **tokens;
    RedirectInfo redirect;
    int is_background;
} Command;

typedef struct {
    pid_t pid;
    char *cmd;
    int is_running;
} Job;

#endif
