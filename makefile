# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

# Output executable
TARGET = orus

# Source files
SRCS = main.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default rule: Build the shell
all: $(TARGET)

# Compile source files into object files
%.o: %.c
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
	rm -f $(OBJS) $(TARGET)

# Force recompilation
rebuild: clean all
