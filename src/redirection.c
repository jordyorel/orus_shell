#include "redirection.h"


void setup_redirections(RedirectInfo *redirect) {
    if (redirect->input_file) {
        redirect->input_fd = open(redirect->input_file, O_RDONLY);
        if (redirect->input_fd == -1 ||
            dup2(redirect->input_fd, STDIN_FILENO) == -1) {
            perror("Input redirection failed");
            exit(1);
        }
    }

    if (redirect->output_file || redirect->append_file) {
        int flags =
            O_WRONLY | O_CREAT | (redirect->is_append ? O_APPEND : O_TRUNC);
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

        redirect->output_fd =
            open(redirect->output_file ? redirect->output_file
                                       : redirect->append_file,
                 flags, mode);

        if (redirect->output_fd == -1 ||
            dup2(redirect->output_fd, STDOUT_FILENO) == -1) {
            perror("Output redirection failed");
            exit(1);
        }

        if (redirect->redirect_stderr &&
            dup2(redirect->output_fd, STDERR_FILENO) == -1) {
            perror("stderr redirection failed");
            exit(1);
        }
    }
}