# Simple Unix Shell

## Overview
This project is a simple Unix shell written in C. It provides an interactive command-line environment that supports executing commands, handling pipelines, managing background jobs, and performing basic input/output redirection.

## Features

### ✅ Command Execution
- Executes Unix commands using `execvp`.
- Supports command-line arguments.

### ✅ Tokenization and Parsing
- Splits user input into tokens while respecting quotes and escape sequences.
- Supports variable expansion (e.g., `$HOME`).
- Expands wildcard patterns (`*` and `?`) using `glob`.

### ✅ Pipelining
- Supports chaining commands using the pipe (`|`) operator.
- Redirects output of one command to the input of another.

### ✅ I/O Redirection
- Input redirection (`<`): Read from a file instead of standard input.
- Output redirection (`>`): Write output to a file (overwrite mode).
- Append redirection (`>>`): Write output to a file (append mode).
- Stderr redirection (`2>&1`): Redirects stderr to stdout.

### ✅ Background Job Execution
- Commands ending with `&` run in the background.
- Supports listing background jobs with `jobs`.

### ✅ Built-in Commands
- `exit` - Exits the shell.
- `cd` - Changes the working directory.
- `clear` - Clears the screen.
- `jobs` - Displays running background jobs.
- `export` - Sets environment variables.

### ✅ Signal Handling
- Handles `SIGINT` (Ctrl+C) to prevent shell termination.
- Gracefully handles child process termination.

## Installation & Usage
### 📥 Installation
1. Clone this repository:
   ```sh
   git clone https://github.com/your-username/simple-shell.git
   cd simple-shell
   ```
2. Compile the shell:
   ```sh
   gcc -o shell main.c -lreadline
   ```
3. Run the shell:
   ```sh
   ./shell
   ```

### 📌 Usage
Once inside the shell, you can:
- Run commands like `ls`, `grep`, `cat`, etc.
- Use pipes: `ls | grep .c`
- Redirect output: `ls > output.txt`
- Run background processes: `sleep 10 &`
- List jobs: `jobs`
- Change directories: `cd /path/to/directory`
- Exit: `exit`

## Limitations & Future Improvements
### ❌ Current Limitations
- No support for command substitution (`$(command)`).
- No job control commands (`fg`, `bg`, `Ctrl+Z`).
- No scripting capabilities (loops, conditionals, functions).
- Limited error handling for syntax errors.

### 🚀 Future Enhancements
- Implement `fg` and `bg` commands for job control.
- Improve error messages and debugging support.
- Add support for command substitution.
- Implement signal handling for stopping/resuming jobs (`Ctrl+Z`).

## Contributing
Feel free to fork the project and submit pull requests! For major changes, please open an issue first to discuss what you’d like to change.

## License
This project is licensed under the MIT License.


