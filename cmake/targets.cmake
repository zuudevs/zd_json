set(ZD_JSON_LIBRARY_TARGET zd_json)

add_library(${ZD_JSON_LIBRARY_TARGET})

target_sources(${ZD_JSON_LIBRARY_TARGET}
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/lexer/tokenizer.cpp"
        "${CMAKE_SOURCE_DIR}/src/lexer/lexer.cpp"
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

function(add_test_target target_name label)
	if(ZD_JSON_BUILD_TESTS)
		add_executable(zd_${target_name}
			"${CMAKE_SOURCE_DIR}/tests/${target_name}.cpp"
		)

		target_link_libraries(zd_${target_name} PRIVATE zd_json::zd_json)
		target_include_directories(zd_${target_name} PRIVATE
			"${CMAKE_SOURCE_DIR}/include"
			"${CMAKE_SOURCE_DIR}/internal"
		)

		zd_json_enable_warnings(zd_${target_name})
		zd_json_enable_sanitizers(zd_${target_name})
		zd_json_enable_coverage(zd_${target_name})

		if(CLANG_TIDY_EXE)
			set_target_properties(zd_${target_name} PROPERTIES
				CXX_CLANG_TIDY "${CLANG_TIDY_CMD}"
			)
		endif()

		add_test(NAME zd_${target_name} COMMAND zd_${target_name})
		set_tests_properties(zd_${target_name} PROPERTIES LABELS "${label}")
	endif()
endfunction()

function(add_benchmark_target target_name)
    if(ZD_JSON_BUILD_BENCHMARKS)
        add_executable(zd_${target_name}
            "${CMAKE_SOURCE_DIR}/benchmarks/${target_name}.cpp"
        )

        target_link_libraries(zd_${target_name} PRIVATE
            zd_json::zd_json
            benchmark::benchmark
        )
        target_include_directories(zd_${target_name} PRIVATE
            "${CMAKE_SOURCE_DIR}/include"
            "${CMAKE_SOURCE_DIR}/internal"
        )

        zd_json_enable_warnings(zd_${target_name})
        zd_json_enable_release_optimizations(zd_${target_name})

        if(CLANG_TIDY_EXE)
            set_target_properties(zd_${target_name} PROPERTIES
                CXX_CLANG_TIDY "${CLANG_TIDY_CMD}"
            )
        endif()
    endif()
endfunction()

add_test_target(json_error_test error)
add_test_target(json_tokenizer_test tokenizer)
add_test_target(json_lexer_test lexer)
add_test_target(json_parser_test parser)

add_benchmark_target(json_tokenizer_benchmarks)
add_benchmark_target(json_lexer_benchmarks)
add_benchmark_target(json_parser_benchmarks)