# https://www.gnu.org/software/make/manual/make.html
# make -recon

PROJECT_NAME ?= gomps4
BUILD_MODE ?= DEBUG
EXECUTABLE ?= $(PROJECT_NAME)

# Define default options
# One of PLATFORM_DESKTOP, PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB
# should have been defined as an arg to make in tasks.json
PLATFORM           ?= PLATFORM_DESKTOP
PLATFORM_OS        ?= LINUX

# libraylib.a is installed in /usr/local/lib
# raylib.h (and raylibmath.h &c) installed in /usr/local/include

# RAYLIB_INSTALL_PATH should be the desired full path to libraylib. No relative paths.
RAYLIB_INSTALL_PATH ?= /home/gilbert/raylib

# RAYLIB_H_INSTALL_PATH locates the installed raylib header and associated source files.
RAYLIB_H_INSTALL_PATH ?= /home/gilbert/raylib/src

# Library type used for raylib: STATIC (.a) or SHARED (.so/.dll)
RAYLIB_LIBTYPE ?= STATIC

LUA_PATH = /home/gilbert/lua-5.4.3/src

SRC_DIR = src
OBJ_DIR = obj

# without CFLAGS += -D_DEFAULT_SOURCE, compiler complains about strdup

CFLAGS += -D$(PLATFORM)
CFLAGS += -std=c99
# https://airbus-seclab.github.io/c-compiler-security/
CFLAGS += -Wall -Wextra -Wpedantic -Werror
#  -Wformat-overflow=2
CFLAGS += -Wformat=2 -Wformat-truncation=2 -Wformat-security 
CFLAGS += -Wnull-dereference -Wtrampolines -Walloca -Warray-bounds=2 -Wimplicit-fallthrough=3 -Wshift-overflow=2 
CFLAGS += -Wcast-qual -Wstringop-overflow=4 -Wlogical-op -Wduplicated-cond -Wduplicated-branches -Wformat-signedness -Wshadow -Wstrict-overflow=5 
CFLAGS += -Wundef -Wstrict-prototypes -Wswitch-default -Wswitch-enum -Wcast-align=strict -Wunused
# if using alloca don't add these
CFLAGS += -Wstack-protector -Wstack-usage=1000000
CFLAGS += -fsanitize=undefined
# https://kristerw.github.io/2021/10/19/fast-math/
CFLAGS += -ffast-math
CFLAGS += -D_FORTIFY_SOURCE=2
CFLAGS += -fstack-protector-strong -fstack-clash-protection -fPIE 

# https://stackoverflow.com/questions/13224209/gcc-wunused-function-not-working-but-other-warnings-are-working/23756859
#CFLAGS += -ffunction-sections -fdata-sections
#CFLAGS += -Wl,--gc-sections -Wl,--print-gc-sections

INCLUDE_PATHS = -I$(RAYLIB_H_INSTALL_PATH) -I$(LUA_PATH)

# Extra flags to give to compilers when they are supposed to invoke the linker, ‘ld’, such as -L
LDFLAGS = -L$(LUA_PATH) -L$(RAYLIB_INSTALL_PATH) -fsanitize=undefined

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
OBJECT_FILES = $(patsubst %.c,%.o,$(SOURCE_FILES))

# $@ filename of target of the rule
# $< name of the first prerequisite

# prepend /usr/bin/time to $(CC)

$(EXECUTABLE): $(OBJECT_FILES) $(HEADER_FILES) Makefile
	$(CC) $(OBJECT_FILES) -o $@ $(LDLIBS) $(LDFLAGS)
#	mkdir -p $(dir $@)

#$(EXECUTABLE): $(SOURCE_FILES) $(HEADER_FILES) Makefile
#	$(CC) -o $@ $(SOURCE_FILES) $(CFLAGS) $(INCLUDE_PATHS) $(LDLIBS) $(LDFLAGS)
#	@ls -al csol

# -c compile and assemble, but do not link
# -o <file> place the output into <file>
# prepend /usr/bin/time to $(CC)
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# compile and link all the .c files
# use -v to see what's happening
#$(EXECUTABLE): $(SOURCE_FILES) $(HEADER_FILES) Makefile
#	$(CC) -o $@ $(SOURCE_FILES) $(CFLAGS) $(INCLUDE_PATHS) $(LDLIBS) $(LDFLAGS)
#	@echo $(PLAT)
#	@ls -al csol

.PHONY: clean check valgrind

clean:
	rm $(EXECUTABLE) *.o

check:
	@echo $(BUILD_MODE)
	@echo
	@echo $(HEADER_FILES)
	@echo
	@echo $(SOURCE_FILES)
	@echo
	@echo $(OBJECT_FILES)

valgrind:
	valgrind --track-origins=yes --leak-check=full -s ./$(EXECUTABLE) --pack=default
