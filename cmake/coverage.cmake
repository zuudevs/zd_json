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