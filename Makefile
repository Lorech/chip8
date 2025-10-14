.DEFAULT_GOAL := run

# Build the project using CMake, and then compile the program.
build:
	@cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
	@cmake --build build -j

# Run the compiled program.
run: build
	@echo "Starting CHIP-8..."
	@./build/chip8

# Clean up the build directory.
clean:
	@echo "Cleaning build directory..."
	@rm -rf build
	@echo "Build has been cleaned."
