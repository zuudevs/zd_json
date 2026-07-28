set(ZD_JSON_LIBRARY_TARGET zd_json)

add_library(${ZD_JSON_LIBRARY_TARGET})

target_sources(${ZD_JSON_LIBRARY_TARGET}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/lexer/tokenizer.cpp"
        "${CMAKE_SOURCE_DIR}/src/lexer/lexer.cpp"
        "${CMAKE_SOURCE_DIR}/src/models/arena.cpp"
        "${CMAKE_SOURCE_DIR}/src/models/value.cpp"
        "${CMAKE_SOURCE_DIR}/src/models/array.cpp"
        "${CMAKE_SOURCE_DIR}/src/models/object.cpp"
        "${CMAKE_SOURCE_DIR}/src/models/document.cpp"
        "${CMAKE_SOURCE_DIR}/src/parser/parse_bool.cpp"
        "${CMAKE_SOURCE_DIR}/src/parser/parse_float.cpp"
        "${CMAKE_SOURCE_DIR}/src/parser/parse_integral.cpp"
        "${CMAKE_SOURCE_DIR}/src/parser/parse_literal.cpp"
        "${CMAKE_SOURCE_DIR}/src/parser/parse_null.cpp"
        "${CMAKE_SOURCE_DIR}/src/parser/parse_string.cpp"
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

function(add_test_target TARGET_NAME LABEL)
	if(ZD_JSON_BUILD_TESTS)
		add_executable(zd_${TARGET_NAME}
			"${CMAKE_SOURCE_DIR}/tests/${TARGET_NAME}.cpp"
		)

		target_link_libraries(zd_${TARGET_NAME} PRIVATE zd_json::zd_json)
		target_include_directories(zd_${TARGET_NAME} PRIVATE
			"${CMAKE_SOURCE_DIR}/include"
			"${CMAKE_SOURCE_DIR}/internal"
		)

		zd_json_enable_warnings(zd_${TARGET_NAME})
		zd_json_enable_sanitizers(zd_${TARGET_NAME})
		zd_json_enable_coverage(zd_${TARGET_NAME})

		if(CLANG_TIDY_EXE)
			set_target_properties(zd_${TARGET_NAME} PROPERTIES
				CXX_CLANG_TIDY "${CLANG_TIDY_CMD}"
			)
		endif()

		add_test(NAME zd_${TARGET_NAME} COMMAND zd_${TARGET_NAME})
		set_tests_properties(zd_${TARGET_NAME} PROPERTIES LABELS "${LABEL}")
	endif()
endfunction()

function(add_benchmark_target TARGET_NAME)
	add_executable(zd_${TARGET_NAME}
        "${CMAKE_SOURCE_DIR}/benchmarks/${TARGET_NAME}.cpp"
    )

    target_link_libraries(zd_${TARGET_NAME} PRIVATE
        zd_json::zd_json
        benchmark::benchmark
    )
    target_include_directories(zd_${TARGET_NAME} PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/internal"
    )

    zd_json_enable_warnings(zd_${TARGET_NAME})
    zd_json_enable_release_optimizations(zd_${TARGET_NAME})

    if(CLANG_TIDY_EXE)
        set_target_properties(zd_${TARGET_NAME} PROPERTIES
            CXX_CLANG_TIDY "${CLANG_TIDY_CMD}"
        )
    endif()
endfunction()

add_test_target(json_error_test error)
add_test_target(json_tokenizer_test tokenizer)
add_test_target(json_lexer_test lexer)
add_test_target(json_parser_test parser)
add_test_target(json_models_test models)

add_benchmark_target(json_tokenizer_benchmarks)
add_benchmark_target(json_lexer_benchmarks)
add_benchmark_target(json_parser_benchmarks)
add_benchmark_target(json_models_benchmarks)