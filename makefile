SHELL=/opt/homebrew/bin/bash

SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

SOURCES := $(wildcard $(SRC_DIR)/*.c)

##### SDL variables #####
# --------------------------------------------------------------------------------------------------

SDL_CC = clang
SDL_CFLAGS = -Wall -g -DSDL
SDL_DFLAGS = -DDEBUG

SDL_OBJ_DIR=$(OBJ_DIR)/sdl
SDL_BIN_DIR=$(BIN_DIR)/sdl

SDL_OBJ := $(SOURCES:$(SRC_DIR)/%.c=$(SDL_OBJ_DIR)/%.o)

##### Apple ][ variables #####
# --------------------------------------------------------------------------------------------------
# Define cl65 as the main compiler/linker
# CC65 = cc65
# CA65 = ca65
CL65 = cl65

# Define c2t: a program to convert binary to a wav
C2T = /Users/garrettbodley/Code/c2t-master/bin/c2t_arm

# Define AppleCommander file
AC = lib/AppleCommander-ac-1.9.0.jar

# Specify the include directory
APL_CFLAGS = -O -t apple2 --start-addr 0x2000 -Wl -D__EXEHDR__=0 -D APPLE2

# Define the directories
APL_OBJ_DIR = $(OBJ_DIR)/apple2
APL_BIN_DIR = $(BIN_DIR)/apple2

# Specify the target system for the cc65 suite, if necessary
APL_TARGET_SYS = apple2

# Start address for the output binary
APL_START_ADDRESS = 2000

# Name of the final executable
APL_TARGET = $(APL_BIN_DIR)/chip8

# APL_ASSEMBLIES := $(patsubst $(SRC_DIR)/%.c,$(APL_OBJ_DIR)/%.s,$(SOURCES))
# APL_OBJECTS := $(patsubst $(APL_OBJ_DIR)/%.s,$(APL_OBJ_DIR)/%.o,$(APL_ASSEMBLIES))

APL_OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(APL_OBJ_DIR)/%.o)

# Define disk image for use with AppleCommander
DISK_IMG = lib/ProDOS_8.dsk
TARGET_DISK = $(APL_BIN_DIR)/chip8.dsk

##### TEST variables #####
# --------------------------------------------------------------------------------------------------

TEST_DIR=tests
TEST_SRC_DIR=$(TEST_DIR)/src
TEST_OBJ_DIR=$(TEST_DIR)/obj
TEST_BIN_DIR=$(TEST_DIR)/bin

TEST_SOURCES := $(wildcard $(TEST_SRC_DIR)/*.c)
TEST_OBJ := $(TEST_SOURCES:$(TEST_SRC_DIR)/%.c=$(TEST_OBJ_DIR)/%.o)
TEST_BINS := $(TEST_SOURCES:$(TEST_SRC_DIR)/%.c=$(TEST_BIN_DIR)/%)

##### Homebrew stuff #####
# --------------------------------------------------------------------------------------------------

BREW_INCLUDE=/opt/homebrew/include
BREW_LIB=/opt/homebrew/lib

##### Top-level make commands #####
# --------------------------------------------------------------------------------------------------

all: $(SDL_BIN_DIR)/chip8

sdl: $(SDL_BIN_DIR)/chip8

apple2: $(APL_TARGET)

wav: $(APL_TARGET).aif

disk: $(TARGET_DISK)

sha-c2t:
	sha1sum $(C2T)

test: $(TEST_BINS) | $(TEST_OBJ)
	for test_bin in $^; do ./$$test_bin; done

##### DEBUG build command #####
#
# Debug statements are guarded behind ifdef clauses
# "debug" and "debug-test" simply define a DEBUG symbol before calling their respective make commands
# --------------------------------------------------------------------------------------------------

debug: SDL_CFLAGS += $(SDL_DFLAGS)
debug: all

debug-test: SDL_CFLAGS += $(SDL_DFLAGS)
debug-test: test

# Log which clang is being used
which-clang:
	@echo $(shell echo `realpath $(shell which clang)`)

# Log which shell is being used
which-shell:
	@echo $(SHELL)

##### Init necessary directories #####
# --------------------------------------------------------------------------------------------------
$(SDL_OBJ_DIR) $(SDL_BIN_DIR) $(TEST_BIN_DIR) $(TEST_OBJ_DIR) $(APL_OBJ_DIR) $(APL_BIN_DIR) $(APL_OUT_DIR):
	mkdir -p $@

##### SDL build process #####
# --------------------------------------------------------------------------------------------------

# Generates SDL specific .o files from .c files
$(SDL_OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(SDL_OBJ_DIR)
	$(SDL_CC) $(SDL_CFLAGS) $(shell sdl2-config --cflags) -c $< -o $@

# Generates SDL specific binary file from obj/sdl/*.o files
$(SDL_BIN_DIR)/chip8: $(SDL_OBJ) | $(SDL_BIN_DIR)
	$(SDL_CC) $(SDL_CFLAGS) $^ -o $@ $(shell sdl2-config --libs)

##### Apple2 build process #####
# --------------------------------------------------------------------------------------------------

# Generate apple2 .s files from .c
# $(APL_OBJ_DIR)/%.s: $(SRC_DIR)/%.c | $(APL_OBJ_DIR)
# 	$(CC65) $(APL_CFLAGS) -t $(APL_TARGET_SYS) -o $@ $<

# Rule to make the objects
$(APL_OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(APL_OBJ_DIR)
	$(CL65) $(APL_CFLAGS) -c -o $@ $<

# Rule to make the target
$(APL_TARGET): $(APL_OBJECTS) | $(APL_BIN_DIR)
	$(CL65) $(APL_CFLAGS) -o $@ $^
	@# $(CL65) $(APL_CFLAGS) -o $@ obj/apple2/main.o

# Rule to make wav from apple2 binary
$(APL_TARGET).aif: $(APL_TARGET)
	$(C2T) -bc8 $(APL_TARGET),$(APL_START_ADDRESS) $(APL_TARGET).aif

# Rule to make disk image from apple2 binary
$(TARGET_DISK): $(APL_TARGET)
	cp $(DISK_IMG) $(TARGET_DISK)
	for f in launcher sysutil fastcopy basic; do java -jar $(AC) -d $(TARGET_DISK) $$f.system; done
	java -jar $(AC) -p $(TARGET_DISK) main.system sys 0x$(APL_START_ADDRESS) < $(APL_TARGET)

##### Test build process #####
# --------------------------------------------------------------------------------------------------

# Generates test specific .o files from test/src/*.c files
$(TEST_OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.c | $(TEST_OBJ_DIR)
	$(SDL_CC) $(SDL_CFLAGS) $(shell sdl2-config --cflags) -I$(SRC_DIR) -I$(BREW_INCLUDE) -c $< -o $@

# Generates test binaries from test/obj/*.o files
$(TEST_BIN_DIR)/%: $(TEST_OBJ_DIR)/%.o $(filter-out $(SDL_OBJ_DIR)/main.o, $(SDL_OBJ)) | $(TEST_BIN_DIR)
	$(SDL_CC) $(SDL_CFLAGS) $(shell sdl2-config --libs) $^ -o $@ -L$(BREW_LIB) -lcriterion

##### Clean commands #####
# --------------------------------------------------------------------------------------------------

clean:
	@[ -n "${BIN_DIR}" ] || { echo "BIN_DIR unset or null"; exit 127; }
	@[ -n "${OBJ_DIR}" ] || { echo "OBJ_DIR unset or null"; exit 127; }
	@[ -n "${TEST_OBJ_DIR}" ] || { echo "TEST_OBJ_DIR unset or null"; exit 127; }
	@[ -n "${TEST_BIN_DIR}" ] || { echo "TEST_BIN_DIR unset or null"; exit 127; }
	@ # echo $(BIN_DIR) $(OBJ_DIR) $(TEST_OBJ_DIR) $(TEST_BIN_DIR) # For debugging purposes
	rm -rf $(BIN_DIR) $(OBJ_DIR) $(TEST_OBJ_DIR) $(TEST_BIN_DIR)

clean-test:
	@[ -n "${TEST_OBJ_DIR}" ] || { echo "TEST_OBJ_DIR unset or null"; exit 127; }
	@[ -n "${TEST_BIN_DIR}" ] || { echo "TEST_BIN_DIR unset or null"; exit 127; }
	@ # echo $(TEST_OBJ_DIR) $(TEST_BIN_DIR)
	rm -rf $(TEST_OBJ_DIR) $(TEST_BIN_DIR)

.PHONY: clean clean-test debug debug-test test