#include "utils.h"

/* Cleanup Functions */
void cleanup_redirections(RedirectInfo *redirect) {
    if (redirect->input_fd > 0) close(redirect->input_fd);
    if (redirect->output_fd > 0) close(redirect->output_fd);
    free(redirect->input_file);
    free(redirect->output_file);
    free(redirect->append_file);
}

void cleanup_command_list(Command *commands, int num_commands) {
    for (int i = 0; i < num_commands; i++) {
        for (int j = 0; commands[i].tokens[j] != NULL; j++) {
            free(commands[i].tokens[j]);
        }
        free(commands[i].tokens);
        cleanup_redirections(&commands[i].redirect);
    }
    free(commands);
}
