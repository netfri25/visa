CC = clang -fcolor-diagnostics
BUILD_DIR = build
INCLUDE_DIR = ./src
CFLAGS = -std=c23 -Wall -Wextra -pedantic -I$(INCLUDE_DIR)

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(SRC))

all: $(BUILD_DIR)/main

$(BUILD_DIR)/main: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

test:
	./run_tests.sh

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
