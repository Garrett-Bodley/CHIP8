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

TEST_SOURCES := $(wildcard $(TEST_SRC_DIR)/*.c)
TEST_OBJ := $(TEST_SOURCES:$(TEST_SRC_DIR)/%.c=$(TEST_OBJ_DIR)/%.o)
TEST_BINS := $(TEST_SOURCES:$(TEST_SRC_DIR)/%.c=$(TEST_BIN_DIR)/%)

SOURCES := $(wildcard $(SRC_DIR)/*.c)
_OBJ := $(notdir $(SOURCES:.c=.o))
OBJ := $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

# TESTS := $(wildcard $(TEST_DIR)/*.c)
# TESTOBJ = $(filter-out $(OBJ_DIR)/main.o, $(OBJ))
# TESTBINS := $(patsubst $(TEST_DIR)/%.c, $(TEST_DIR)/bin/%, $(TESTS))

$(SRC_DIR) $(OBJ_DIR) $(BIN_DIR) $(TEST_DIR) $(TEST_BIN_DIR) $(TEST_OBJ_DIR) $(TEST_SRC_DIR):
	mkdir -p $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TESTDIR)/bin/%: $(TEST_DIR)/%.c $(TEST_OBJ) | $(TESTDIR)/bin
	$(CC) $(CFLAGS) $< $(TEST_OBJ) -o $@ -L$(BREW_LIB) -I$(BREW_INCLUDE) -lcriterion

test: $(TEST_BIN_DIR) $(TESTBINS)
	for criterion_test in $(TESTBINS); do ./$$criterion_test; done

.PHONY: clean test

clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/* $(TEST_BIN_DIR)/* $(TEST_OBJ_DIR)/*.o
