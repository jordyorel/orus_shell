# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g -Iinclude

# Output executable
TARGET = orus

# Source files (find all .c files in the src directory)
SRCS = $(shell find src -name '*.c')

# Object files (redirecting to the build directory)
OBJS = $(SRCS:src/%.c=build/%.o)

# Default rule: Build the shell
all: $(TARGET)

# Compile source files into object files
build/%.o: src/%.c
	@mkdir -p build  # Create the build directory if it doesn't exist
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to create the executable with Readline support
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) -lreadline  # Link against Readline

# Run the shell
run: $(TARGET)
	./$(TARGET)

# Check for memory leaks using macOS `leaks`
memcheck: $(TARGET)
	leaks --atExit -- ./$(TARGET)

# Clean build artifacts
clean:
	rm -rf build $(TARGET)

# Force recompilation
rebuild: clean all
