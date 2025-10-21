include(FetchContent)

set(RAYLIB_VERSION 5.5)

FetchContent_Declare(raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
)

FetchContent_MakeAvailable(raylib)

# Core Library (shared across targets)

file(GLOB CORE_SOURCES CONFIGURE_DEPENDS *.c)
list(FILTER CORE_SOURCES EXCLUDE REGEX "main\\.c$")

add_library(${CORE_LIB} STATIC ${CORE_SOURCES})

target_include_directories(${CORE_LIB} PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Desktop Emulator

add_executable(${EMULATOR_EXE} main.c)

target_include_directories(${EMULATOR_EXE} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${EMULATOR_EXE} PRIVATE
    ${CORE_LIB}
    raylib
)

if (${PLATFORM} STREQUAL "Web")
    set_target_properties(${EMULATOR_EXE} PROPERTIES SUFFIX ".html") # Tell Emscripten to build an example.html file.
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY -s GL_ENABLE_GET_PROC_ADDRESS=1")
endif()

if (APPLE)
    target_link_libraries(${EMULATOR_EXE} PRIVATE
        "-framework IOKit"
        "-framework Cocoa"
        "-framework OpenGL"
    )
endif()

set_target_properties(${EMULATOR_EXE} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
