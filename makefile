CC = clang
CFLAGS = -Wall -g

SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

TEST_DIR=tests
TEST_SRC_DIR=tests/src
TEST_OBJ_DIR=tests/obj
TEST_BIN_DIR=tests/bin

BREW_INCLUDE=/opt/homebrew/include
BREW_LIB=/opt/homebrew/lib

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

TEST_SOURCES := $(wildcard $(TEST_SRC_DIR)/*.c)
TEST_OBJ := $(TEST_SOURCES:$(TEST_SRC_DIR)/%.c=$(TEST_OBJ_DIR)/%.o)
TEST_BINS := $(TEST_SOURCES:$(TEST_SRC_DIR)/%.c=$(TEST_BIN_DIR)/%)

all: $(BIN_DIR)/main

which-clang:
	@echo $(shell echo `realpath $(shell which clang)`)

$(OBJ_DIR) $(BIN_DIR) $(TEST_BIN_DIR) $(TEST_OBJ_DIR):
	mkdir -p $@

# compilation of source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# compilation of test source files
$(TEST_OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.c | $(TEST_OBJ_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -I -I$(BREW_INCLUDE) -c $< -o $@

# linking of test binaries, excluding mian.o
$(TEST_BIN_DIR)/%: $(TEST_OBJ_DIR)/%.o $(filter-out $(OBJ_DIR)/main.o, $(OBJ))
	$(CC) $(CFLAGS) $^ -o $@ -L$(BREW_LIB) -lcriterion


test: $(TESTBINS)
	for test_bin in $^; do ./$$test_bin; done

$(BIN_DIR)/main: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean test

clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/* $(TEST_OBJ_DIR)/*.o $(TEST_BIN_DIR)/*
