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
        "${CMAKE_SOURCE_DIR}/src/parser/parse_object.cpp"
        "${CMAKE_SOURCE_DIR}/src/parser/parse_array.cpp"
        "${CMAKE_SOURCE_DIR}/src/parser/parse_value.cpp"
		"${CMAKE_SOURCE_DIR}/src/serializer/serializer.cpp"
		"${CMAKE_SOURCE_DIR}/src/serializer/serialize_number.cpp"
		"${CMAKE_SOURCE_DIR}/src/serializer/serialize_string.cpp"
		"${CMAKE_SOURCE_DIR}/src/zd_json/json.cpp"
)

# NOTE: internal/ is exposed PUBLIC (not PRIVATE) because the public
# umbrella header <zd_json/json.hpp> aliases models::Document/Value/
# Array/Object and serializer::SerializeOptions directly rather than
# duplicating their definitions -- so any consumer that includes
# zd_json/json.hpp needs internal/ on its include path too. The headers
# under internal/ are still not part of the supported API surface: only
# the names re-exported from zd_json/json.hpp (the zuu:: aliases and
# Parse()/Serialize*()) are meant to be used directly by consumers.
target_include_directories(${ZD_JSON_LIBRARY_TARGET}
    PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_BINARY_DIR}/include"
        "${CMAKE_SOURCE_DIR}/internal"
)

target_compile_features(${ZD_JSON_LIBRARY_TARGET} PUBLIC cxx_std_23)

zd_json_enable_warnings(${ZD_JSON_LIBRARY_TARGET})
zd_json_enable_release_optimizations(${ZD_JSON_LIBRARY_TARGET})
zd_json_enable_sanitizers(${ZD_JSON_LIBRARY_TARGET})
zd_json_enable_coverage(${ZD_JSON_LIBRARY_TARGET})

if(ZD_JSON_BUILD_FUZZERS)
    target_compile_options(${ZD_JSON_LIBRARY_TARGET} PRIVATE
        -fsanitize-coverage=inline-8bit-counters,pc-table,trace-cmp
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )
endif()

if(CLANG_TIDY_EXE)
    set_target_properties(${ZD_JSON_LIBRARY_TARGET} PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_CMD}"
    )
endif()

add_library(zd_json::zd_json ALIAS ${ZD_JSON_LIBRARY_TARGET})

function(add_test_target type_test target_name src_dir)
	if(ZD_JSON_BUILD_TESTS)
		file(GLOB TEST_SRCS
			CONFIGURE_DEPENDS
			"${CMAKE_CURRENT_SOURCE_DIR}/tests/${type_test}/${src_dir}/*.cpp"
		)

		add_executable(zd_${target_name}
			"${CMAKE_SOURCE_DIR}/tests/${type_test}/${target_name}.cpp"
			"${TEST_SRCS}"
		)

		target_link_libraries(zd_${target_name} PRIVATE
			zd_json::zd_json
			GTest::gtest
			GTest::gtest_main
		)
		target_include_directories(zd_${target_name} PRIVATE
			"${CMAKE_SOURCE_DIR}/include"
			"${CMAKE_SOURCE_DIR}/internal"
			"${CMAKE_SOURCE_DIR}/tests/${type_test}/${src_dir}"
		)

		zd_json_enable_warnings(zd_${target_name})
		zd_json_enable_sanitizers(zd_${target_name})
		zd_json_enable_coverage(zd_${target_name})

		if(CLANG_TIDY_EXE)
			set_target_properties(zd_${target_name} PROPERTIES
				CXX_CLANG_TIDY "${CLANG_TIDY_CMD};-checks=-readability-function-cognitive-complexity,-readability-magic-numbers,-readability-identifier-length,-readability-uppercase-literal-suffix,-readability-qualified-auto"
			)
		endif()

		add_test(NAME zd_${target_name} COMMAND zd_${target_name})
	endif()
endfunction()

function(add_benchmark_target target_name src_dir)
    if(ZD_JSON_BUILD_BENCHMARKS)
		file(GLOB BM_SRCS
			CONFIGURE_DEPENDS
			"${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/${src_dir}/*.cpp"
		)

        add_executable(zd_${target_name}
            "${CMAKE_SOURCE_DIR}/benchmarks/${target_name}.cpp"
			"${BM_SRCS}"
        )

        target_link_libraries(zd_${target_name} PRIVATE
            zd_json::zd_json
            benchmark::benchmark
        )
        target_include_directories(zd_${target_name} PRIVATE
            "${CMAKE_SOURCE_DIR}/include"
            "${CMAKE_SOURCE_DIR}/internal"
			"${CMAKE_SOURCE_DIR}/benchmarks/${src_dir}"
        )

        target_compile_definitions(zd_${target_name} PRIVATE
            ZD_JSON_SAMPLES_DIR="${CMAKE_SOURCE_DIR}/assets/samples"
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

add_test_target(unit json_error_test error)
add_test_target(unit json_tokenizer_test tokenizer)
add_test_target(unit json_lexer_test lexer)
add_test_target(unit json_parser_test parser)
add_test_target(unit json_models_test models)
add_test_target(unit json_serializer_test serializer)
add_test_target(unit json_api_test api)

add_test_target(integration json_parsing_integration_test parsing)
add_test_target(integration json_dom_integration_test dom)
add_test_target(integration json_serialization_integration_test serialization)
add_test_target(integration json_pipeline_integration_test pipeline)

add_benchmark_target(json_tokenizer_benchmarks tokenizer)
add_benchmark_target(json_lexer_benchmarks lexer)
add_benchmark_target(json_parser_benchmarks parser)
add_benchmark_target(json_models_benchmarks models)
add_benchmark_target(json_serializer_benchmarks serializer)
add_benchmark_target(json_real_datasets_benchmarks real_datasets)

add_fuzz_target(parse)
add_fuzz_target(roundtrip)