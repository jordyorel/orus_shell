/*This file contains the Oru's original code 
*   that has been spilted to multiple files in src
*/


// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <ctype.h>
// #include <sys/stat.h>
// #include <errno.h>
// #include <fcntl.h>
// #include <glob.h>
// #include <readline/history.h>
// #include <readline/readline.h>
// #include <signal.h>
// #include <sys/types.h>
// #include <sys/wait.h>

// /* Constants and Color Definitions */
// #define MAX_COMMAND_LENGTH 1024
// #define MAX_TOKENS 128
// #define MAX_PIPES 10
// #define MAX_JOBS 100

// #define COLOR_RED "\033[31m"
// #define COLOR_GREEN "\033[32m"
// #define COLOR_BLUE "\033[34m"
// #define COLOR_RESET "\033[0m"

// /* Type Definitions */
// typedef struct {
//     char *input_file;
//     char *output_file;
//     char *append_file;
//     int input_fd;
//     int output_fd;
//     int is_append;
//     int redirect_stderr;
// } RedirectInfo;

// typedef struct {
//     char **tokens;
//     RedirectInfo redirect;
//     int is_background;
// } Command;

// typedef struct {
//     pid_t pid;
//     char *cmd;
//     int is_running;
// } Job;

// /* Global Variables */
// static Job jobs[MAX_JOBS];
// static int job_count = 0;

// /* Utility Function Prototypes */
// char **tokenize_input(const char *line, int *num_tokens);
// char **expand_wildcards(char **tokens, int *num_tokens);
// Command *parse_commands(char *input, int *num_commands);
// void execute_pipeline(Command *commands, int num_commands);
// void cleanup_redirections(RedirectInfo *redirect);
// void cleanup_command_list(Command *commands, int num_commands);

// /* Job Control Functions */
// void add_job(pid_t pid, char *cmd, int is_running) {
//     if (job_count < MAX_JOBS) {
//         jobs[job_count].pid = pid;
//         jobs[job_count].cmd = strdup(cmd);
//         jobs[job_count].is_running = is_running;
//         job_count++;
//     } else {
//         fprintf(stderr, COLOR_RED "Job list is full.\n" COLOR_RESET);
//     }
// }

// void update_job_status() {
//     for (int i = 0; i < job_count; i++) {
//         if (jobs[i].is_running) {
//             int status;
//             if (waitpid(jobs[i].pid, &status, WNOHANG) > 0) {
//                 jobs[i].is_running = 0;
//                 printf("[%d] Done: %s\n", i + 1, jobs[i].cmd);
//             }
//         }
//     }
// }

// void list_jobs() {
//     for (int i = 0; i < job_count; i++) {
//         printf("[%d] %s %s\n", i + 1, jobs[i].is_running ? "Running" : "Done",
//                jobs[i].cmd);
//     }
// }

// /* Signal Handling */
// void handle_sigint(int sig __attribute__((unused))) {
//     printf("\n" "> ");
//     fflush(stdout);
// }

// /* Tokenization and Expansion */
// char **tokenize_input(const char *line, int *num_tokens) {
//     char **tokens = malloc(MAX_TOKENS * sizeof(char *));
//     int ntokens = 0;
//     const char *p = line;

//     while (*p) {
//         while (*p && isspace((unsigned char)*p)) p++;
//         if (!*p) break;

//         char buffer[MAX_COMMAND_LENGTH] = {0};
//         int buf_index = 0;
//         int expand = 1;

//         if (*p == '"' || *p == '\'') {
//             char quote = *p++;
//             expand = (quote == '"');
//             while (*p && *p != quote) {
//                 if (*p == '\\' && quote == '"') {
//                     buffer[buf_index++] = *(++p);
//                 } else {
//                     buffer[buf_index++] = *p;
//                 }
//                 p++;
//             }
//             if (*p == quote) p++;
//         } else {
//             while (*p && !isspace((unsigned char)*p)) {
//                 buffer[buf_index++] = *p++;
//             }
//         }
//         buffer[buf_index] = '\0';

//         // Variable expansion
//         if (expand) {
//             char expanded[MAX_COMMAND_LENGTH] = {0};
//             int ei = 0;
//             for (int i = 0; buffer[i]; i++) {
//                 if (buffer[i] == '$') {
//                     char varname[128] = {0};
//                     int vi = 0;
//                     i++;

//                     if (buffer[i] == '{') {
//                         i++;
//                         while (buffer[i] && buffer[i] != '}') {
//                             varname[vi++] = buffer[i++];
//                         }
//                     } else {
//                         while (buffer[i] &&
//                                (isalnum((unsigned char)buffer[i]) ||
//                                 buffer[i] == '_')) {
//                             varname[vi++] = buffer[i++];
//                         }
//                         i--;
//                     }
//                     varname[vi] = '\0';

//                     char *value = getenv(varname);
//                     if (value) {
//                         strcpy(expanded + ei, value);
//                         ei += strlen(value);
//                     }
//                 } else {
//                     expanded[ei++] = buffer[i];
//                 }
//             }
//             expanded[ei] = '\0';
//             tokens[ntokens++] = strdup(expanded);
//         } else {
//             tokens[ntokens++] = strdup(buffer);
//         }
//     }
//     tokens[ntokens] = NULL;
//     *num_tokens = ntokens;
//     return tokens;
// }

// char **expand_wildcards(char **tokens, int *num_tokens) {
//     char **new_tokens = malloc(MAX_TOKENS * sizeof(char *));
//     if (new_tokens == NULL) {
//         perror("malloc failed");  // Handle malloc failure
//         exit(1);                  // Or return an error code if you prefer
//     }
//     int ntokens = 0;

//     for (int i = 0; tokens[i] != NULL; i++) {
//         if (strpbrk(tokens[i], "*?") != NULL) {
//             glob_t glob_results;
//             int ret = glob(tokens[i], 0, NULL, &glob_results);
//             if (ret == 0) {  // Successful glob
//                 if (glob_results.gl_pathc > 0) {
//                     for (size_t j = 0; j < glob_results.gl_pathc; j++) {
//                         new_tokens[ntokens++] =
//                             strdup(glob_results.gl_pathv[j]);
//                         if (new_tokens[ntokens - 1] == NULL) {
//                             perror("strdup failed");
//                             exit(1);
//                         }
//                     }
//                 } else {  // No match found: Keep the original token
//                     new_tokens[ntokens++] = strdup(tokens[i]);
//                     if (new_tokens[ntokens - 1] == NULL) {
//                         perror("strdup failed");
//                         exit(1);
//                     }
//                 }
//                 globfree(&glob_results);
//             } else {  // glob failed
//                 fprintf(stderr, COLOR_RED "glob failed: %s\n" COLOR_RESET,
//                         strerror(ret));  // More informative error message
//                 new_tokens[ntokens++] = strdup(
//                     tokens[i]);  // Keep original token even if glob fails
//                 if (new_tokens[ntokens - 1] == NULL) {
//                     perror("strdup failed");
//                     exit(1);
//                 }
//             }
//         } else {
//             new_tokens[ntokens++] = strdup(tokens[i]);
//             if (new_tokens[ntokens - 1] == NULL) {
//                 perror("strdup failed");
//                 exit(1);
//             }
//         }
//     }
//     new_tokens[ntokens] = NULL;
//     *num_tokens = ntokens;

//     // Free original tokens (after they've been copied)
//     for (int i = 0; tokens[i] != NULL; i++) {
//         free(tokens[i]);
//     }
//     free(tokens);

//     return new_tokens;
// }

// /* Command Parsing */
// Command* parse_commands(char *input, int *num_commands) {
//     Command *commands = malloc(MAX_PIPES * sizeof(Command));
//     *num_commands = 0;

//     char *saveptr1;
//     char *cmd_str = strtok_r(input, "|", &saveptr1);

//     while (cmd_str != NULL && *num_commands < MAX_PIPES) {
//         // Trim whitespace
//         while (*cmd_str == ' ') cmd_str++;
//         size_t len = strlen(cmd_str);
//         while (len > 0 && isspace((unsigned char)cmd_str[len - 1])) {
//             cmd_str[len - 1] = '\0';
//             len--;
//         }

//         // Check for empty command
//         if (strlen(cmd_str) == 0) {
//             fprintf(stderr,
//                     COLOR_RED "Error: empty command in pipeline\n" COLOR_RESET);
//             cmd_str = strtok_r(NULL, "|", &saveptr1);
//             continue;
//         }

//         // Initialize redirection info
//         commands[*num_commands].redirect = (RedirectInfo){0};
//         commands[*num_commands].redirect.input_fd = -1;
//         commands[*num_commands].redirect.output_fd = -1;
//         commands[*num_commands].is_background = 0;

//         // Tokenize and expand
//         int tok_count = 0;
//         char **raw_tokens = tokenize_input(cmd_str, &tok_count);
//         raw_tokens = expand_wildcards(raw_tokens, &tok_count);

//         // Process tokens for redirections and background
//         char **tokens = malloc(MAX_TOKENS * sizeof(char *));
//         int final_tok = 0;

//         for (int i = 0; i < tok_count;) {
//             if (strcmp(raw_tokens[i], "&") == 0) {
//                 commands[*num_commands].is_background = 1;
//                 free(raw_tokens[i++]);
//                 continue;
//             }

//             // Input redirection
//             if (strcmp(raw_tokens[i], "<") == 0 && i + 1 < tok_count) {
//                 commands[*num_commands].redirect.input_file =
//                     strdup(raw_tokens[i + 1]);
//                 free(raw_tokens[i]);
//                 free(raw_tokens[i + 1]);
//                 i += 2;
//                 continue;
//             }

//             // Output redirection
//             if (strcmp(raw_tokens[i], ">") == 0 && i + 1 < tok_count) {
//                 commands[*num_commands].redirect.output_file =
//                     strdup(raw_tokens[i + 1]);
//                 free(raw_tokens[i]);
//                 free(raw_tokens[i + 1]);
//                 i += 2;
//                 continue;
//             }

//             // Append redirection
//             if (strcmp(raw_tokens[i], ">>") == 0 && i + 1 < tok_count) {
//                 commands[*num_commands].redirect.append_file =
//                     strdup(raw_tokens[i + 1]);
//                 commands[*num_commands].redirect.is_append = 1;
//                 free(raw_tokens[i]);
//                 free(raw_tokens[i + 1]);
//                 i += 2;
//                 continue;
//             }

//             // Stderr redirection
//             if (strcmp(raw_tokens[i], "2>&1") == 0) {
//                 commands[*num_commands].redirect.redirect_stderr = 1;
//                 free(raw_tokens[i++]);
//                 continue;
//             }

//             // Add to tokens
//             tokens[final_tok++] = raw_tokens[i++];
//         }
//         tokens[final_tok] = NULL;
//         free(raw_tokens);

//         // Check if the command is empty after processing
//         if (final_tok == 0) {
//             fprintf(stderr, COLOR_RED "Error: empty command.\n" COLOR_RESET);
//             free(tokens);
//             cmd_str = strtok_r(NULL, "|", &saveptr1);
//             continue;
//         }

//         commands[*num_commands].tokens = tokens;
//         (*num_commands)++;

//         cmd_str = strtok_r(NULL, "|", &saveptr1);
//     }

//     return commands;
// }

// /* Redirection Setup */
// void setup_redirections(RedirectInfo *redirect) {
//     if (redirect->input_file) {
//         redirect->input_fd = open(redirect->input_file, O_RDONLY);
//         if (redirect->input_fd == -1 ||
//             dup2(redirect->input_fd, STDIN_FILENO) == -1) {
//             perror("Input redirection failed");
//             exit(1);
//         }
//     }

//     if (redirect->output_file || redirect->append_file) {
//         int flags =
//             O_WRONLY | O_CREAT | (redirect->is_append ? O_APPEND : O_TRUNC);
//         mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

//         redirect->output_fd =
//             open(redirect->output_file ? redirect->output_file
//                                        : redirect->append_file,
//                  flags, mode);

//         if (redirect->output_fd == -1 ||
//             dup2(redirect->output_fd, STDOUT_FILENO) == -1) {
//             perror("Output redirection failed");
//             exit(1);
//         }

//         if (redirect->redirect_stderr &&
//             dup2(redirect->output_fd, STDERR_FILENO) == -1) {
//             perror("stderr redirection failed");
//             exit(1);
//         }
//     }
// }

// /* Pipeline Execution */
// void execute_pipeline(Command *commands, int num_commands) {
//     if (num_commands == 0) return;

//     int pipes[MAX_PIPES - 1][2];
//     pid_t pids[MAX_PIPES];

//     // Create pipes
//     for (int i = 0; i < num_commands - 1; i++) {
//         if (pipe(pipes[i]) == -1) {
//             perror("pipe");
//             return;
//         }
//     }

//     // Fork and execute each command
//     for (int i = 0; i < num_commands; i++) {
//         if (commands[i].tokens[0] == NULL) {
//             fprintf(stderr, COLOR_RED "Error: empty command.\n" COLOR_RESET);
//             continue;
//         }

//         pids[i] = fork();
//         if (pids[i] == -1) {
//             perror("fork");
//             return;
//         }

//         if (pids[i] == 0) {  // Child process
//             // Pipe redirections
//             if (i > 0) {
//                 if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
//                     perror("dup2 input");
//                     exit(1);
//                 }
//             }
//             if (i < num_commands - 1) {
//                 if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
//                     perror("dup2 output");
//                     exit(1);
//                 }
//             }

//             // Close all pipe file descriptors
//             for (int j = 0; j < num_commands - 1; j++) {
//                 close(pipes[j][0]);
//                 close(pipes[j][1]);
//             }

//             // File redirections for first and last commands
//             if (i == 0 || i == num_commands - 1)
//                 setup_redirections(&commands[i].redirect);

//             // Execute command
//             execvp(commands[i].tokens[0], commands[i].tokens);
//             perror("execvp");
//             exit(1);
//         }
//     }

//     // Close pipe file descriptors in parent
//     for (int i = 0; i < num_commands - 1; i++) {
//         close(pipes[i][0]);
//         close(pipes[i][1]);
//     }

//     // Wait for commands
//     if (!commands[num_commands - 1].is_background) {
//         for (int i = 0; i < num_commands; i++) {
//             waitpid(pids[i], NULL, 0);
//         }
//     } else {
//         add_job(pids[num_commands - 1], commands[num_commands - 1].tokens[0],
//                 1);
//     }
// }

// /* Continued from previous artifact */
// void handle_builtin_command(char **tokens) {
//     if (strcmp(tokens[0], "exit") == 0) {
//         printf(COLOR_BLUE "Exiting shell..\n" COLOR_RESET);
//         exit(0);
//     } else if (strcmp(tokens[0], "cd") == 0) {
//         if (tokens[1] == NULL) {
//             fprintf(stderr,
//                     COLOR_RED "cd: Destination not specified.\n" COLOR_RESET);
//             return;
//         }
//         if (chdir(tokens[1]) != 0) {
//             perror("cd");
//         }
//     } else if (strcmp(tokens[0], "clear") == 0) {
//         system("clear");
//     } else if (strcmp(tokens[0], "jobs") == 0) {
//         list_jobs();
//     } else if (strcmp(tokens[0], "export") == 0) {
//         if (tokens[1] == NULL) {
//             fprintf(stderr,
//                     COLOR_RED "export: Variable not specified.\n" COLOR_RESET);
//             return;
//         }
//         char *envstr = strdup(tokens[1]);
//         if (envstr == NULL) {
//             perror("strdup");
//             return;
//         }
//         if (putenv(envstr) != 0) {
//             perror("export");
//             free(envstr);  // Free envstr on error
//             return;
//         }
//     }
// }

// /* Cleanup Functions */
// void cleanup_redirections(RedirectInfo *redirect) {
//     if (redirect->input_fd > 0) close(redirect->input_fd);
//     if (redirect->output_fd > 0) close(redirect->output_fd);
//     free(redirect->input_file);
//     free(redirect->output_file);
//     free(redirect->append_file);
// }

// void cleanup_command_list(Command *commands, int num_commands) {
//     for (int i = 0; i < num_commands; i++) {
//         for (int j = 0; commands[i].tokens[j] != NULL; j++) {
//             free(commands[i].tokens[j]);
//         }
//         free(commands[i].tokens);
//         cleanup_redirections(&commands[i].redirect);
//     }
//     free(commands);
// }

// /* Main Function */
// int main() {
//     const char *prompt = "> ";

//     // Set up signal handling
//     signal(SIGINT, handle_sigint);

//     // Main shell loop
//     while (1) {
//         // Update background job statuses
//         update_job_status();

//         // Read input
//         char *input = readline(prompt);

//         // Handle EOF (Ctrl+D)
//         if (!input) {
//             printf("\n");
//             break;
//         }

//         // Skip empty input
//         if (strlen(input) == 0) {
//             free(input);
//             continue;
//         }

//         // Add to readline history
//         add_history(input);

//         // Parse commands
//         int num_commands = 0;
//         Command *commands = parse_commands(input, &num_commands);

//         // Handle parsing errors
//         if (!commands) {
//             fprintf(stderr,
//                     COLOR_RED "Invalid command line input.\n" COLOR_RESET);
//             free(input);
//             continue;
//         }

//         // Handle built-in commands
//         if (num_commands == 1 && commands[0].tokens[0] != NULL) {
//             char *cmd = commands[0].tokens[0];
//             if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "cd") == 0 ||
//                 strcmp(cmd, "clear") == 0 || strcmp(cmd, "jobs") == 0 ||
//                 strcmp(cmd, "export") == 0) {
//                 handle_builtin_command(commands[0].tokens);
//             } else {
//                 execute_pipeline(commands, num_commands);
//             }
//         } else {
//             execute_pipeline(commands, num_commands);
//         }

//         // Cleanup
//         cleanup_command_list(commands, num_commands);
//         free(input);
//     }

//     return 0;
// }