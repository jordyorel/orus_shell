#include "signal_handler.h"

void handle_sigint(int sig __attribute__((unused))) {
    printf("\n" "> ");
    fflush(stdout);
}