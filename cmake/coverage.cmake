# cmake/coverage.cmake

function(zd_json_enable_coverage target_name)
    if(NOT ZD_JSON_ENABLE_COVERAGE)
        return()
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(${target_name} PRIVATE
            -fprofile-instr-generate
            -fcoverage-mapping
        )
        target_link_options(${target_name} PRIVATE
            -fprofile-instr-generate
        )
    endif()
endfunction()

if(ZD_JSON_ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    find_program(LLVM_PROFDATA_EXE llvm-profdata)
    find_program(LLVM_COV_EXE llvm-cov)

    if(LLVM_PROFDATA_EXE AND LLVM_COV_EXE)
        add_custom_target(coverage-report
            COMMAND ${LLVM_PROFDATA_EXE} merge -sparse ${CMAKE_BINARY_DIR}/default.profraw -o ${CMAKE_BINARY_DIR}/code_coverage.profdata
            COMMAND ${LLVM_COV_EXE} report $<TARGET_FILE:zd_json_api_test> -instr-profile=${CMAKE_BINARY_DIR}/code_coverage.profdata
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating terminal code coverage report..."
        )

        add_custom_target(coverage-html
            COMMAND ${LLVM_PROFDATA_EXE} merge -sparse ${CMAKE_BINARY_DIR}/default.profraw -o ${CMAKE_BINARY_DIR}/code_coverage.profdata
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/out/coverage
            COMMAND ${LLVM_COV_EXE} show $<TARGET_FILE:zd_json_api_test> -instr-profile=${CMAKE_BINARY_DIR}/code_coverage.profdata -format=html -output-dir=${CMAKE_BINARY_DIR}/out/coverage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating HTML code coverage report in ${CMAKE_BINARY_DIR}/out/coverage..."
        )
    endif()
endif()