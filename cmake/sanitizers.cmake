function(zd_json_enable_sanitizers target_name)
    if(NOT ZD_JSON_ENABLE_SANITIZERS)
        return()
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target_name} PRIVATE
            -fsanitize=address
            -fsanitize=undefined
            -fno-omit-frame-pointer
        )
        target_link_options(${target_name} PRIVATE
            -fsanitize=address
            -fsanitize=undefined
        )
    endif()
endfunction()