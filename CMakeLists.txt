cmake_minimum_required(VERSION 3.16)
project(chip8 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CORE_LIB chip8_core)  # Main emulator functionality, shared across targets
set(EMULATOR_EXE chip8)   # An executable for the emulator targeting desktop
set(TEST_EXE chip8_tests) # An executable for unit testing the emulator

add_subdirectory(src)

# MARK: Build Targets

option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    add_subdirectory(test)
endif()

# MARK: Build-specific Defaults

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ENABLE_LOGS ON)
endif()

# MARK: Customizable Compiler Flags

option(ENABLE_LOGS "Enable runtime logging" OFF)
if(ENABLE_LOGS)
    target_compile_definitions({PROJECT_NAME} PRIVATE ENABLE_LOGS)
endif()
