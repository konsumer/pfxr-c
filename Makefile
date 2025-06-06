CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -fPIC
INCLUDES = -Iinclude
LIBS = -lm

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
EXAMPLE_DIR = examples

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Library name
LIB_NAME = libpfxr
STATIC_LIB = $(LIB_NAME).a
SHARED_LIB = $(LIB_NAME).so

# Example programs
EXAMPLE_SOURCES = $(wildcard $(EXAMPLE_DIR)/*.c)
EXAMPLES = $(EXAMPLE_SOURCES:$(EXAMPLE_DIR)/%.c=$(BUILD_DIR)/%)

.PHONY: all clean static shared examples install

all: static shared examples

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build static library
static: $(BUILD_DIR)/$(STATIC_LIB)

$(BUILD_DIR)/$(STATIC_LIB): $(OBJECTS)
	ar rcs $@ $^

# Build shared library
shared: $(BUILD_DIR)/$(SHARED_LIB)

$(BUILD_DIR)/$(SHARED_LIB): $(OBJECTS)
	$(CC) -shared -o $@ $^ $(LIBS)

# Build examples
examples: $(EXAMPLES)

$(BUILD_DIR)/%: $(EXAMPLE_DIR)/%.c $(BUILD_DIR)/$(STATIC_LIB) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< -L$(BUILD_DIR) -lpfxr $(LIBS)

# Build api_demo specifically
$(BUILD_DIR)/api_demo: $(EXAMPLE_DIR)/api_demo.c $(BUILD_DIR)/$(STATIC_LIB) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< -L$(BUILD_DIR) -lpfxr $(LIBS)

# Install (optional)
install: static shared
	sudo cp $(BUILD_DIR)/$(STATIC_LIB) /usr/local/lib/
	sudo cp $(BUILD_DIR)/$(SHARED_LIB) /usr/local/lib/
	sudo cp $(INC_DIR)/pfxr.h /usr/local/include/
	sudo ldconfig

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: all

# Test target
test: examples
	@echo "Running basic test..."
	$(BUILD_DIR)/simple_example

# Help
help:
	@echo "PFXR C Library Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build static and shared libraries, plus examples"
	@echo "  static   - Build static library only"
	@echo "  shared   - Build shared library only"
	@echo "  examples - Build example programs"
	@echo "  install  - Install libraries and headers to system"
	@echo "  clean    - Remove all build files"
	@echo "  debug    - Build with debug symbols"
	@echo "  test     - Run basic functionality test"
	@echo "  help     - Show this help message"