cmake_minimum_required(VERSION 3.16)
project(chip8 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

# MARK: Build options

option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    add_subdirectory(test)
endif()

# MARK: Build type handling

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ENABLE_LOGS ON)
endif()

# MARK: Compiler flags

option(ENABLE_LOGS "Enable runtime logging" OFF)
if(ENABLE_LOGS)
    target_compile_definitions(chip8 PRIVATE ENABLE_LOGS)
endif()
