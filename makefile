SHELL=/opt/homebrew/bin/bash

SDL_CC = clang
SDL_CFLAGS = -Wall -g -DSDL
SDL_DFLAGS = -DDEBUG

SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

SDL_OBJ_DIR=obj/sdl
SDL_BIN_DIR=bin/sdl

TEST_DIR=tests
TEST_SRC_DIR=tests/src
TEST_OBJ_DIR=tests/obj
TEST_BIN_DIR=tests/bin

BREW_INCLUDE=/opt/homebrew/include
BREW_LIB=/opt/homebrew/lib

SDL_SOURCES := $(wildcard $(SRC_DIR)/*.c)
SDL_OBJ := $(SDL_SOURCES:$(SRC_DIR)/%.c=$(SDL_OBJ_DIR)/%.o)

TEST_SOURCES := $(wildcard $(TEST_SRC_DIR)/*.c)
TEST_OBJ := $(TEST_SOURCES:$(TEST_SRC_DIR)/%.c=$(TEST_OBJ_DIR)/%.o)
TEST_BINS := $(TEST_SOURCES:$(TEST_SRC_DIR)/%.c=$(TEST_BIN_DIR)/%)

all: $(SDL_BIN_DIR)/chip8

sdl: $(SDL_BIN_DIR)/chip8

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
$(SDL_OBJ_DIR) $(SDL_BIN_DIR) $(TEST_BIN_DIR) $(TEST_OBJ_DIR):
	mkdir -p $@

##### SDL build process #####
# --------------------------------------------------------------------------------------------------

# Generates SDL specific .o files from .c files
$(SDL_OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(SDL_OBJ_DIR)
	$(SDL_CC) $(SDL_CFLAGS) $(shell sdl2-config --cflags) -c $< -o $@

# Generates SDL specific binary file from obj/sdl/*.o files
$(SDL_BIN_DIR)/chip8: $(SDL_OBJ) | $(SDL_BIN_DIR)
	$(SDL_CC) $(SDL_CFLAGS) $^ -o $@ $(shell sdl2-config --libs)

##### Test build process #####
# --------------------------------------------------------------------------------------------------

# Generates test specific .o files from test/src/*.c files
$(TEST_OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.c | $(TEST_OBJ_DIR)
	$(SDL_CC) $(SDL_CFLAGS) $(shell sdl2-config --cflags) -I$(SRC_DIR) -I$(BREW_INCLUDE) -c $< -o $@

# Generates test binaries from test/obj/*.o files
$(TEST_BIN_DIR)/%: $(TEST_OBJ_DIR)/%.o $(filter-out $(SDL_OBJ_DIR)/main.o, $(SDL_OBJ)) | $(TEST_BIN_DIR)
	$(SDL_CC) $(SDL_CFLAGS) $(shell sdl2-config --libs) $^ -o $@ -L$(BREW_LIB) -lcriterion

test: $(TEST_BINS) | $(TEST_OBJ)
	for test_bin in $^; do ./$$test_bin; done

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