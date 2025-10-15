.DEFAULT_GOAL := run

BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin

# Set up the project using CMake.
configure:
	@echo "Configuring CMake..."
	@cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug

# Compile the CMake project.
build: configure
	@echo "Building project..."
	@cmake --build $(BUILD_DIR) -j

# Run the compiled program.
run: build
	@echo "Starting CHIP-8..."
	@$(BIN_DIR)/chip8

# Run the compiled test suite.
test: build
	@echo "Running Test Suite..."
	@$(BIN_DIR)/chip8_tests

# Clean up the build directory.
clean:
	@echo "Cleaning build directory..."
	@rm -rf build
	@echo "Build has been cleaned."
