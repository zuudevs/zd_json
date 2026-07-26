set(ZD_JSON_LIBRARY_TARGET zd_json)

add_library(${ZD_JSON_LIBRARY_TARGET})

target_sources(${ZD_JSON_LIBRARY_TARGET}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/lexer/tokenizer.cpp"
)

target_include_directories(${ZD_JSON_LIBRARY_TARGET}
    PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/include"
    PRIVATE
        "${CMAKE_SOURCE_DIR}/internal"
)

target_compile_features(${ZD_JSON_LIBRARY_TARGET} PUBLIC cxx_std_23)

zd_json_enable_warnings(${ZD_JSON_LIBRARY_TARGET})
zd_json_enable_release_optimizations(${ZD_JSON_LIBRARY_TARGET})
zd_json_enable_sanitizers(${ZD_JSON_LIBRARY_TARGET})
zd_json_enable_coverage(${ZD_JSON_LIBRARY_TARGET})

if(CLANG_TIDY_EXE)
    set_target_properties(${ZD_JSON_LIBRARY_TARGET} PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_CMD}"
    )
endif()

add_library(zd_json::zd_json ALIAS ${ZD_JSON_LIBRARY_TARGET})

if(ZD_JSON_BUILD_TESTS)
    add_executable(zd_json_error_test
        "${CMAKE_SOURCE_DIR}/tests/json_error_test.cpp"
    )

    target_link_libraries(zd_json_error_test PRIVATE zd_json::zd_json)
    target_include_directories(zd_json_error_test PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/internal"
    )

    zd_json_enable_warnings(zd_json_error_test)
    zd_json_enable_sanitizers(zd_json_error_test)
    zd_json_enable_coverage(zd_json_error_test)

    if(CLANG_TIDY_EXE)
        set_target_properties(zd_json_error_test PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_CMD}"
        )
    endif()

    add_test(NAME zd_json_error_test COMMAND zd_json_error_test)

    add_executable(zd_json_tokenizer_test
        "${CMAKE_SOURCE_DIR}/tests/json_tokenizer_test.cpp"
    )

    target_link_libraries(zd_json_tokenizer_test PRIVATE zd_json::zd_json)
    target_include_directories(zd_json_tokenizer_test PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/internal"
    )

    zd_json_enable_warnings(zd_json_tokenizer_test)
    zd_json_enable_sanitizers(zd_json_tokenizer_test)
    zd_json_enable_coverage(zd_json_tokenizer_test)

    if(CLANG_TIDY_EXE)
        set_target_properties(zd_json_tokenizer_test PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_CMD}"
        )
    endif()

    add_test(NAME zd_json_tokenizer_test COMMAND zd_json_tokenizer_test)
endif()

if(ZD_JSON_BUILD_BENCHMARKS)
    add_executable(zd_json_tokenizer_benchmarks
        "${CMAKE_SOURCE_DIR}/benchmarks/json_tokenizer_benchmark.cpp"
    )

    target_link_libraries(zd_json_tokenizer_benchmarks PRIVATE
        zd_json::zd_json
        benchmark::benchmark
    )
    target_include_directories(zd_json_tokenizer_benchmarks PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/internal"
    )

    zd_json_enable_warnings(zd_json_tokenizer_benchmarks)
    zd_json_enable_release_optimizations(zd_json_tokenizer_benchmarks)

    if(CLANG_TIDY_EXE)
        set_target_properties(zd_json_tokenizer_benchmarks PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_CMD}"
        )
    endif()
endif()