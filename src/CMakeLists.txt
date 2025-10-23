add_subdirectory(core)
add_subdirectory(platform)

if(BUILD_DESKTOP)
    add_executable(${DESKTOP_EXE} main.c)

    target_link_libraries(${DESKTOP_EXE} PRIVATE
        ${CORE_LIB}
        ${DESKTOP_LIB}
    )

    set_target_properties(${DESKTOP_EXE} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()
