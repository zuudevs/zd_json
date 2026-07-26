set(ZD_JSON_LIBRARY_TARGET zd_json)

add_library(${ZD_JSON_LIBRARY_TARGET})

target_sources(${ZD_JSON_LIBRARY_TARGET}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/init.cpp"
)

target_include_directories(${ZD_JSON_LIBRARY_TARGET}
    PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
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
    add_executable(zd_json_tests
        "${CMAKE_SOURCE_DIR}/tests/json_error_test.cpp"
    )

    target_link_libraries(zd_json_tests PRIVATE zd_json::zd_json)
    target_include_directories(zd_json_tests PRIVATE "${CMAKE_SOURCE_DIR}/include")

    zd_json_enable_warnings(zd_json_tests)
    zd_json_enable_sanitizers(zd_json_tests)
    zd_json_enable_coverage(zd_json_tests)

    if(CLANG_TIDY_EXE)
        set_target_properties(zd_json_tests PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_CMD}"
        )
    endif()

    add_test(NAME zd_json_tests COMMAND zd_json_tests)
endif()

if(ZD_JSON_BUILD_BENCHMARKS)
    add_executable(zd_json_benchmarks
        "${CMAKE_SOURCE_DIR}/benchmarks/init.cpp"
    )

    target_link_libraries(zd_json_benchmarks PRIVATE
        zd_json::zd_json
        benchmark::benchmark
    )

    zd_json_enable_warnings(zd_json_benchmarks)
    zd_json_enable_release_optimizations(zd_json_benchmarks)

    if(CLANG_TIDY_EXE)
        set_target_properties(zd_json_benchmarks PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_CMD}"
        )
    endif()
endif()