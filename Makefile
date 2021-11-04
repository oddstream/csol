# https://www.gnu.org/software/make/manual/make.html

PROJECT_NAME ?= csol
EXECUTABLE = $(PROJECT_NAME)

BUILD_MODE ?= DEBUG

# libraylib.a is installed in /usr/local/lib
# raylib.h (and raylibmath.h &c) installed in /usr/local/include

# RAYLIB_INSTALL_PATH should be the desired full path to libraylib. No relative paths.
RAYLIB_INSTALL_PATH ?= /usr/local/lib
# RAYLIB_H_INSTALL_PATH locates the installed raylib header and associated source files.
RAYLIB_H_INSTALL_PATH ?= /usr/local/include

LUA_PATH = /home/gilbert/lua-5.4.3/src

SRC_DIR = src
OBJ_DIR = obj

CFLAGS += -std=c99 -D_DEFAULT_SOURCE -DLIL_ENABLE_POOLS
# https://airbus-seclab.github.io/c-compiler-security/
CFLAGS += -Werror
CFLAGS += -Wall -Wextra -Wpedantic -Wformat=2 -Wformat-overflow=2 -Wformat-truncation=2 -Wformat-security -Wnull-dereference -Wtrampolines -Walloca -Warray-bounds=2 -Wimplicit-fallthrough=3 -Wshift-overflow=2 -Wcast-qual -Wstringop-overflow=4 -Wlogical-op -Wduplicated-cond -Wduplicated-branches -Wformat-signedness -Wshadow -Wstrict-overflow=5 -Wundef -Wstrict-prototypes -Wswitch-default -Wswitch-enum -Wcast-align=strict
# if using alloca don't add these
CFLAGS += -Wstack-protector -Wstack-usage=1000000
CFLAGS += -fsanitize=undefined
# https://kristerw.github.io/2021/10/19/fast-math/
CFLAGS += -ffast-math
CFLAGS += -D_FORTIFY_SOURCE=2
CFLAGS += -fstack-protector-strong -fstack-clash-protection -fPIE 

INCLUDE_PATHS = -I$(RAYLIB_H_INSTALL_PATH) -I$(LUA_PATH)

# Extra flags to give to compilers when they are supposed to invoke the linker, ‘ld’, such as -L
LDFLAGS = -L$(LUA_PATH) 

# Library flags or names given to compilers when they are supposed to invoke the linker, ‘ld’.
LDLIBS = -lraylib -llua -lGL -lm -ldl -lpthread 
# libdl is the dynamic linking library, omitting it produces:
# /usr/bin/ld: /usr/local/lib/libraylib.a(rglfw.o): undefined reference to symbol 'dlclose@@GLIBC_2.2.5'
# /usr/bin/ld: /lib/x86_64-linux-gnu/libdl.so.2: error adding symbols: DSO missing from command line
# collect2: error: ld returned 1 exit status

# these two didn't seem to do anything: -lrt -lX11

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -ggdb3 -O0 -D_DEBUG
else
    CFLAGS += -s -O2
endif

HEADER_FILES = $(wildcard *.h)
SOURCE_FILES = $(wildcard *.c)
OBJECT_FILES = $(SOURCE_FILES:%=$(OBJ_DIR)/%.o)

# $@ filename of target of the rule
# $< name of the first prerequisite

#$(OBJ_DIR)/%.c.o: %.c
#	mkdir -p $(dir $@)
#	$(CC) $(CFLAGS)  $(INCLUDE_PATHS) -c $< -o $@

# compile and link all the .c files
# use -v to see what's happening
$(EXECUTABLE): $(SOURCE_FILES) $(HEADER_FILES) Makefile
	$(CC) -o $@ $(SOURCE_FILES) $(CFLAGS) $(INCLUDE_PATHS) $(LDLIBS) $(LDFLAGS)
	@ls -al csol

.PHONY: clean check

clean:
	rm $(EXECUTABLE) $(OBJ_DIR)/*.o

check:
	@echo $(BUILD_MODE)
	@echo $(HEADER_FILES)
	@echo
	@echo $(SOURCE_FILES)
	@echo
	@echo $(OBJECT_FILES)
