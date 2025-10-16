include(FetchContent)

find_package(Python3
    REQUIRED COMPONENTS Interpreter
)

FetchContent_Declare(Unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.6.1
)

FetchContent_MakeAvailable(Unity)

set(TOOLS_DIR ${PROJECT_SOURCE_DIR}/tools)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set(RUNNERS_DIR ${TEST_DIR}/runners)

include_directories(${unity_SOURCE_DIR}/src)
include_directories(${unity_SOURCE_DIR}/extras/fixture/src)
include_directories(${PROJECT_SOURCE_DIR}/src)

set(GENERATOR_SCRIPT ${TOOLS_DIR}/generate_unity_runners.py)
set(GENERATED_SOURCES
    ${RUNNERS_DIR}/all_tests.c
    ${RUNNERS_DIR}/test_font_runner.c
    ${RUNNERS_DIR}/test_memory_runner.c
    ${RUNNERS_DIR}/test_stack_runner.c
)

add_custom_command(
    OUTPUT ${GENERATED_SOURCES}
    COMMAND ${Python3_EXECUTABLE} ${GENERATOR_SCRIPT}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generating Unity test runners"
    VERBATIM
)

add_custom_target(generate_tests
    DEPENDS ${GENERATED_SOURCES}
)

file(GLOB CHIP8_SOURCES ${PROJECT_SOURCE_DIR}/src/*.c)
list(FILTER CHIP8_SOURCES EXCLUDE REGEX "main\\.c$")
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ${TEST_DIR}/*.c)

add_compile_definitions(UNITY_FIXTURE_NO_EXTRAS)

add_executable(chip8_tests
    ${CHIP8_SOURCES}
    ${TEST_SOURCES}
    ${GENERATED_SOURCES}
    ${unity_SOURCE_DIR}/src/unity.c
    ${unity_SOURCE_DIR}/extras/fixture/src/unity_fixture.c
)

add_dependencies(chip8_tests generate_tests)

set_target_properties(chip8_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_custom_target(tests
    COMMAND $<TARGET_FILE:chip8_tests>
    DEPENDS chip8_tests
)
