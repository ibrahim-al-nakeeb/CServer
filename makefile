# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iheaders

# Directories
SRC_DIR = sources
OBJ_DIR = obj
BIN = server

# Source files and object files
SRCS = $(wildcard $(SRC_DIR)/*.c) main.c
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SRCS)))

# Default target
all: $(BIN)

# Create binary
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Create obj directory and compile each source file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/main.o: main.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure obj directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: all clean