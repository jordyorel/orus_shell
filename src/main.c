#include "builtins.h"
#include "executor.h"
#include "jobs.h"
#include "orus.h"
#include "parser.h"
#include "redirection.h"
#include "signal_handler.h"
#include "utils.h"

int main() {
    const char *prompt = "> ";

    // Set up signal handling
    signal(SIGINT, handle_sigint);

    // Main shell loop
    while (1) {
        // Update background job statuses
        update_job_status();

        // Read input
        char *input = readline(prompt);

        // Handle EOF (Ctrl+D)
        if (!input) {
            printf("\n");
            break;
        }

        // Skip empty input
        if (strlen(input) == 0) {
            free(input);
            continue;
        }

        // Add to readline history
        add_history(input);

        // Parse commands
        int num_commands = 0;
        Command* commands = parse_commands(input, &num_commands);

        // Handle parsing errors
        if (!commands) {
            fprintf(stderr,
                    COLOR_RED "Invalid command line input.\n" COLOR_RESET);
            free(input);
            continue;
        }

        // Handle built-in commands
        if (num_commands == 1 && commands[0].tokens[0] != NULL) {
            char *cmd = commands[0].tokens[0];
            if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "cd") == 0 ||
                strcmp(cmd, "clear") == 0 || strcmp(cmd, "jobs") == 0 ||
                strcmp(cmd, "export") == 0) {
                handle_builtin_command(commands[0].tokens);
            } else {
                execute_pipeline(commands, num_commands);
            }
        } else {
            execute_pipeline(commands, num_commands);
        }

        // Cleanup
        cleanup_command_list(commands, num_commands);
        free(input);
    }

    return 0;
}