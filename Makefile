# Detect OS
UNAME_S := $(shell uname -s)

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
INCLUDES = -Iinclude
LIBS = -lm

# Platform-specific settings
ifeq ($(UNAME_S),Linux)
    CFLAGS += -fPIC
    SHARED_EXT = .so
    SHARED_FLAGS = -shared
    INSTALL_PREFIX = /usr/local
    LDCONFIG = sudo ldconfig
endif
ifeq ($(UNAME_S),Darwin)
    CFLAGS += -fPIC
    SHARED_EXT = .dylib
    SHARED_FLAGS = -dynamiclib -install_name @rpath/$(LIB_NAME)$(SHARED_EXT)
    INSTALL_PREFIX = /usr/local
    LDCONFIG =
endif
ifeq ($(OS),Windows_NT)
    SHARED_EXT = .dll
    STATIC_EXT = .lib
    SHARED_FLAGS = -shared -Wl,--out-implib,$(BUILD_DIR)/$(LIB_NAME).lib
    INSTALL_PREFIX = C:/Program Files
    LDCONFIG =
else
    STATIC_EXT = .a
endif

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
STATIC_LIB = $(LIB_NAME)$(STATIC_EXT)
SHARED_LIB = $(LIB_NAME)$(SHARED_EXT)

# Example programs
EXAMPLE_SOURCES = $(wildcard $(EXAMPLE_DIR)/*.c)
EXAMPLES = $(EXAMPLE_SOURCES:$(EXAMPLE_DIR)/%.c=$(BUILD_DIR)/%)

.PHONY: help all clean static shared examples install


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

all: static shared examples

# Create build directory
$(BUILD_DIR):
ifeq ($(OS),Windows_NT)
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
else
	mkdir -p $(BUILD_DIR)
endif

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
	$(CC) $(SHARED_FLAGS) -o $@ $^ $(LIBS)

# Build examples
examples: $(EXAMPLES)

$(BUILD_DIR)/%: $(EXAMPLE_DIR)/%.c $(BUILD_DIR)/$(STATIC_LIB) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< -L$(BUILD_DIR) -lpfxr $(LIBS)

# Build api_demo specifically
$(BUILD_DIR)/api_demo: $(EXAMPLE_DIR)/api_demo.c $(BUILD_DIR)/$(STATIC_LIB) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< -L$(BUILD_DIR) -lpfxr $(LIBS)

# Install (optional)
install: static shared
ifeq ($(OS),Windows_NT)
	copy $(BUILD_DIR)\$(STATIC_LIB) "$(INSTALL_PREFIX)\lib\"
	copy $(BUILD_DIR)\$(SHARED_LIB) "$(INSTALL_PREFIX)\lib\"
	copy $(INC_DIR)\pfxr.h "$(INSTALL_PREFIX)\include\"
else
	sudo cp $(BUILD_DIR)/$(STATIC_LIB) $(INSTALL_PREFIX)/lib/
	sudo cp $(BUILD_DIR)/$(SHARED_LIB) $(INSTALL_PREFIX)/lib/
	sudo cp $(INC_DIR)/pfxr.h $(INSTALL_PREFIX)/include/
	$(LDCONFIG)
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
debug: all

# Test target
test: examples
	@echo "Running basic test..."
	$(BUILD_DIR)/simple_example
