if(ZD_JSON_ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)

    if(CLANG_TIDY_EXE)
        set(CLANG_TIDY_CMD "${CLANG_TIDY_EXE};--header-filter=${CMAKE_SOURCE_DIR}/include/.*")
    endif()
endif()