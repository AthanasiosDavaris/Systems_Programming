CC       := gcc
CFLAGS   := -Wall -Wextra -O0 -g -std=c11
CPPFLAGS := -Iinclude

BIN_DIR   := bin
BUILD_DIR := build

# Avoid TAB issues: use '>' as recipe prefix (GNU make)
.RECIPEPREFIX := >

PROGRAMS := buggy proc_inspector hello hello_errno cleanup_demo

all: $(PROGRAMS)

$(BIN_DIR):
>mkdir -p $(BIN_DIR)

$(BUILD_DIR):
>mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
>$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Link rule: same-name .o -> executable
$(BIN_DIR)/%: $(BUILD_DIR)/%.o | $(BIN_DIR)
>$(CC) $(CFLAGS) $< -o $@

# Named program targets (so 'make buggy' works)
buggy: $(BIN_DIR)/buggy
proc_inspector: $(BIN_DIR)/proc_inspector
hello: $(BIN_DIR)/hello
hello_errno: $(BIN_DIR)/hello_errno
cleanup_demo: $(BIN_DIR)/cleanup_demo

run-buggy: buggy
>./$(BIN_DIR)/buggy

run-proc: proc_inspector
>./$(BIN_DIR)/proc_inspector

run-hello: hello
>./$(BIN_DIR)/hello

run-errno: hello_errno
>./$(BIN_DIR)/hello_errno

run-cleanup: cleanup_demo
>./$(BIN_DIR)/cleanup_demo /etc/hostname

clean:
>rm -rf $(BIN_DIR) $(BUILD_DIR)

.PHONY: all clean run-buggy run-proc run-hello run-errno run-cleanup \
        buggy proc_inspector hello hello_errno cleanup_demo
