CC := gcc

SRC_DIR = ./src
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj

CFLAGS = -Wall -Werror -Iinclude

SRC_FILES = $(shell find $(SRC_DIR) -name "*.c")

SRC_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(SRC_OBJS)
	$(CC) -o grafo $^

clean:
	rm -rf $(BUILD_DIR)
	rm grafo