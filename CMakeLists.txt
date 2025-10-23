cmake_minimum_required(VERSION 3.16)
project(chip8 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CORE_LIB lib_chip8_core)       # Implementation of the CHIP-8
set(DESKTOP_LIB lib_chip8_desktop) # The backend for the desktop emulator
set(DESKTOP_EXE exe_chip8_desktop) # The desktop emulator
set(TEST_EXE exe_chip8_tests)      # Unit tests

add_subdirectory(src)

option(BUILD_DESKTOP "Build desktop executable" ON)
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()
