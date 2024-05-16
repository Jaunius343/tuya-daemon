INCLUDE_DIRS = -I $(UTILS_DIR)
LIB_DIRS = 	-llink_core \
			-lmiddleware_implementation \
			-lplatform_port \
			-lutils_modules \
			-largp \
			-lubus \
			-lubox \
			-lblobmsg_json

CFLAGS = -Wall -std=c11 $(INCLUDE_DIRS)
LDFLAGS = $(LIB_DIRS)

SRC_DIR := src
OBJ_DIR := obj
UTILS_DIR := utils

SRCS := $(wildcard $(SRC_DIR)/*.c)
UTILS := $(wildcard $(UTILS_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
UTIL_OBJS := $(patsubst $(UTILS_DIR)/%.c,$(OBJ_DIR)/%.o,$(UTILS))

EXECUTABLE := tuya_daemon



.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS) $(UTIL_OBJS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(UTILS_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@




clean:
	rm -f $(OBJ_DIR)/*.o $(EXECUTABLE)



