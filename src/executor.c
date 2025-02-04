#include "executor.h"
#include "redirection.h"
#include "jobs.h"

void execute_pipeline(Command *commands, int num_commands) {
    if (num_commands == 0) return;

    int pipes[MAX_PIPES - 1][2];
    pid_t pids[MAX_PIPES];

    // Create pipes
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return;
        }
    }

    // Fork and execute each command
    for (int i = 0; i < num_commands; i++) {
        if (commands[i].tokens[0] == NULL) {
            fprintf(stderr, COLOR_RED "Error: empty command.\n" COLOR_RESET);
            continue;
        }

        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            return;
        }

        if (pids[i] == 0) {  // Child process
            // Pipe redirections
            if (i > 0) {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                    perror("dup2 input");
                    exit(1);
                }
            }
            if (i < num_commands - 1) {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2 output");
                    exit(1);
                }
            }

            // Close all pipe file descriptors
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // File redirections for first and last commands
            if (i == 0 || i == num_commands - 1)
                setup_redirections(&commands[i].redirect);

            // Execute command
            execvp(commands[i].tokens[0], commands[i].tokens);
            perror("execvp");
            exit(1);
        }
    }

    // Close pipe file descriptors in parent
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for commands
    if (!commands[num_commands - 1].is_background) {
        for (int i = 0; i < num_commands; i++) {
            waitpid(pids[i], NULL, 0);
        }
    } else {
        add_job(pids[num_commands - 1], commands[num_commands - 1].tokens[0],
                1);
    }
}
