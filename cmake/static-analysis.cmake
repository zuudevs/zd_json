if(ZD_JSON_ENABLE_STATIC_ANALYSIS AND CLANG_TIDY_EXE)
    file(GLOB_RECURSE ALL_SOURCE_FILES CONFIGURE_DEPENDS
        "${CMAKE_SOURCE_DIR}/include/*.hpp"
        "${CMAKE_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_SOURCE_DIR}/tests/*.cpp"
        "${CMAKE_SOURCE_DIR}/benchmarks/*.cpp"
        "${CMAKE_SOURCE_DIR}/example/*.cpp"
    )

    add_custom_target(tidy
        COMMAND ${CLANG_TIDY_EXE} -p=${CMAKE_BINARY_DIR} --fix ${ALL_SOURCE_FILES}
        COMMENT "Fixing source files with clang-tidy"
    )

    add_custom_target(tidy-check
        COMMAND ${CLANG_TIDY_EXE} -p=${CMAKE_BINARY_DIR} --warnings-as-errors=* ${ALL_SOURCE_FILES}
        COMMENT "Checking source files with clang-tidy"
    )
endif()