TARGET = $(BUILD_DIR)/programa

CC := gcc

SRC_DIR = ./src
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj

CFLAGS = -Wall -Werror -Iinclude

SRC_FILES = $(shell find $(SRC_DIR) -name "*.c")

SRC_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

ifneq ($(findstring 1, $(VERBOSE)),)
    Q =
else
    Q = @
    MAKEFLAGS += --quiet
    MAKEFLAGS += --no-print-directory
endif

$(TARGET): $(SRC_OBJS)
	@echo "  BUILD  $@"
	$(Q)$(CC) -o $@ $^
	@echo "  BIN:   $@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "  CC     $<"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

all: $(TARGET)

clean:
	rm -rf $(BUILD_DIR)
