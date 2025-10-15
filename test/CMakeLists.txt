include(FetchContent)

FetchContent_Declare(
    Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.6.1
)

FetchContent_MakeAvailable(Unity)

include_directories(${unity_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src)

file(GLOB CHIP8_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)
list(FILTER CHIP8_SOURCES EXCLUDE REGEX "main\\.c$")
file(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test_*.c)

add_executable(chip8_tests
    ${CHIP8_SOURCES}
    ${TEST_SOURCES}
    ${unity_SOURCE_DIR}/src/unity.c
)

set_target_properties(chip8_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_target(
    tests
    COMMAND ${CMAKE_BINARY_DIR}/bin/chip8_tests
    DEPENDS chip8_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
