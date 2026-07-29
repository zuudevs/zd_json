if(ZD_JSON_ENABLE_CLANG_FORMAT)
    find_program(CLANG_FORMAT_EXE NAMES clang-format)

    if(CLANG_FORMAT_EXE)
        file(GLOB_RECURSE ALL_SOURCE_FILES CONFIGURE_DEPENDS
            "${CMAKE_SOURCE_DIR}/benchmarks/*.cpp"
            "${CMAKE_SOURCE_DIR}/include/*.hpp"
            "${CMAKE_SOURCE_DIR}/internal/*.hpp"
            "${CMAKE_SOURCE_DIR}/src/*.cpp"
            "${CMAKE_SOURCE_DIR}/tests/fuzz/*.cpp"
            "${CMAKE_SOURCE_DIR}/tests/integration/**"
            "${CMAKE_SOURCE_DIR}/tests/unit/**"
            "${CMAKE_SOURCE_DIR}/example/*.cpp"
        )

        add_custom_target(format
            COMMAND ${CLANG_FORMAT_EXE} -i -style=file ${ALL_SOURCE_FILES}
            COMMENT "Formatting source files with clang-format"
        )

        add_custom_target(format-check
            COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror -style=file ${ALL_SOURCE_FILES}
            COMMENT "Checking source formatting"
        )
    endif()
endif()