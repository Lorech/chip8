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

# Runner Generation

set(RUNNER_GEN generate_runners)

set(GENERATOR_SCRIPT ${TOOLS_DIR}/generate_unity_runners.py)
set(GENERATED_SOURCES
    ${RUNNERS_DIR}/all_tests.c
    ${RUNNERS_DIR}/test_chip8_runner.c
    ${RUNNERS_DIR}/test_font_runner.c
)

add_custom_command(
    OUTPUT ${GENERATED_SOURCES}
    COMMAND ${Python3_EXECUTABLE} ${GENERATOR_SCRIPT}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generating Unity test runners"
    VERBATIM
)

add_custom_target(${RUNNER_GEN}
    DEPENDS ${GENERATED_SOURCES}
)

# Test Executable

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ${TEST_DIR}/*.c)

add_compile_definitions(UNITY_FIXTURE_NO_EXTRAS)

add_executable(${TEST_EXE}
    ${TEST_SOURCES}
    ${GENERATED_SOURCES}
    ${unity_SOURCE_DIR}/src/unity.c
    ${unity_SOURCE_DIR}/extras/fixture/src/unity_fixture.c
)

add_dependencies(${TEST_EXE} ${RUNNER_GEN})

target_include_directories(${TEST_EXE} PRIVATE
    ${unity_SOURCE_DIR}/src
    ${unity_SOURCE_DIR}/extras/fixture/src
)

target_link_libraries(${TEST_EXE} PRIVATE
    ${CORE_LIB}
)

set_target_properties(${TEST_EXE} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
