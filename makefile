CC = clang
CFLAGS = -Wall -g

# Target executable and source file
TARGET = chip8
SRC = $(TARGET).c

# Output directory
OUT_DIR = output

# Rule for making the target
$(TARGET): $(SRC)
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -o $(OUT_DIR)/$(TARGET) $(SRC)

# Clean rule
clean:
	rm -rf $(OUT_DIR)/$(TARGET)
