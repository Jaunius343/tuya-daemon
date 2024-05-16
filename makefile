INCLUDE_DIRS = -I $(UTILS_DIR) -I tuya-iot-core-sdk/utils -I tuya-iot-core-sdk/include -I tuya-iot-core-sdk/libraries/coreJSON/source/include -I tuya-iot-core-sdk/interface
LIB_DIRS = -L tuya-iot-core-sdk/build/lib -llink_core -lmiddleware_implementation -lplatform_port -lutils_modules 

# CFLAGS = -Wall -Wextra -std=c11 $(INCLUDE_DIRS)
CFLAGS = -Wall -std=c11 $(INCLUDE_DIRS)
LDFLAGS = $(LIB_DIRS)

LIB_SRC := tuya-iot-core-sdk/build/lib
LIB_DEST := /usr/local/lib
# INC_DIR := ../include
SRC_DIR := src
OBJ_DIR := obj
UTILS_DIR := utils

SRCS := $(wildcard $(SRC_DIR)/*.c)
UTILS := $(wildcard $(UTILS_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
UTIL_OBJS := $(patsubst $(UTILS_DIR)/%.c,$(OBJ_DIR)/%.o,$(UTILS))
LIBS := $(wildcard $(LIB_SRC)/*.so)
LIB_FILES := $(notdir $(wildcard $(LIB_SRC)/*.so))

# check for getting dependencies to prevent cloning sdk each time make is called to build the program
GET_DEPENDENCIES := .DEPENDENCIES_INSTALLED

EXECUTABLE := tuya_daemon
# EXEC_DIR := ../$(EXECUTABLE)







.PHONY: all clean get-dependencies clone patch build install-libs uninstall-libs

all: $(GET_DEPENDENCIES) $(EXECUTABLE)

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






$(GET_DEPENDENCIES):
	$(MAKE) clone patch build
	touch $(GET_DEPENDENCIES)

clone:
	git clone https://github.com/tuya/tuya-iot-core-sdk

patch:
	cp tuya_shared_lib.patch tuya-iot-core-sdk && \
	cd tuya-iot-core-sdk && \
	patch -p1 -i tuya_shared_lib.patch

build:
	mkdir -p tuya-iot-core-sdk/build && \
	cd tuya-iot-core-sdk/build && \
	cmake .. && \
	make




install-libs: | $(LIB_DEST)
	install -D -m 644 $(LIBS) $(LIB_DEST)

$(LIB_DEST):
	install -d $(LIB_DEST)

uninstall-libs:
	cd $(LIB_DEST) && \
	rm -f $(LIB_FILES)


