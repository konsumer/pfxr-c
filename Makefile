# PFXR Single-Header Library Makefile
# ===================================

# Detect OS for platform-specific settings
UNAME_S := $(shell uname -s)

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LIBS = -lm

# Platform-specific settings
ifeq ($(UNAME_S),Linux)
    # Linux settings
endif
ifeq ($(UNAME_S),Darwin)
    # macOS settings
endif
ifeq ($(OS),Windows_NT)
    # Windows settings
endif

# Directories
BUILD_DIR = build
EXAMPLE_DIR = examples

# Example programs
EXAMPLE_SOURCES = $(wildcard $(EXAMPLE_DIR)/*.c)
EXAMPLES = $(EXAMPLE_SOURCES:$(EXAMPLE_DIR)/%.c=$(BUILD_DIR)/%)

.PHONY: all examples clean test help install

# Default target
all: examples

# Help target
help:
	@echo "PFXR Single-Header Library Build System"
	@echo "========================================"
	@echo ""
	@echo "Usage: Include pfxr.h in your project with:"
	@echo "  #define PFXR_IMPLEMENTATION"
	@echo "  #include \"pfxr.h\""
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build example programs"
	@echo "  examples - Build example programs"
	@echo "  test     - Run basic functionality test"
	@echo "  clean    - Remove all build files"
	@echo "  install  - Install header to system"
	@echo "  help     - Show this help message"


# Create build directory
$(BUILD_DIR):
ifeq ($(OS),Windows_NT)
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
else
	mkdir -p $(BUILD_DIR)
endif

# Build examples - each example compiles the entire library
examples: $(EXAMPLES)

$(BUILD_DIR)/%: $(EXAMPLE_DIR)/%.c pfxr.h | $(BUILD_DIR)
	@echo "Compiling single-header example: $*"
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

# Test target
test: $(BUILD_DIR)/simple_example
	@echo "Running basic test..."
	@echo "NOTE: Make sure to run this from the pfxr-c directory"
	$(BUILD_DIR)/simple_example

# Install header to system
install: pfxr.h
ifeq ($(OS),Windows_NT)
	copy pfxr.h "C:\Program Files\include\"
else
	sudo cp pfxr.h /usr/local/include/
	@echo "Header installed to /usr/local/include/pfxr.h"
	@echo "You can now use: #include <pfxr.h>"
endif

# Clean build files
clean:
ifeq ($(OS),Windows_NT)
	if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	del /q *.wav 2>nul || true
else
	rm -rf $(BUILD_DIR) *.wav
endif

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: examples

# Validate header syntax
validate:
	@echo "Validating pfxr.h syntax..."
	$(CC) $(CFLAGS) -fsyntax-only -x c pfxr.h
	@echo "Header syntax validation passed!"
