# cmake/fuzzing.cmake
#
# Wires up LLVM's libFuzzer for the harnesses under tests/fuzz. Fuzz
# targets are opt-in (ZD_JSON_BUILD_FUZZERS, default OFF) and only
# buildable with Clang: libFuzzer is a Clang/compiler-rt feature
# (-fsanitize=fuzzer) with no GCC or MSVC equivalent, so we fail loudly
# rather than silently skipping the targets if someone enables the
# option with the wrong compiler.

if(ZD_JSON_BUILD_FUZZERS)
    add_compile_options(-fno-sanitize-coverage=stack-depth)
endif()

function(add_fuzz_target name)
    if(NOT ZD_JSON_BUILD_FUZZERS)
        return()
    endif()

    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR
            "ZD_JSON_BUILD_FUZZERS requires Clang (libFuzzer is a Clang/"
            "compiler-rt feature). Reconfigure with "
            "-DCMAKE_CXX_COMPILER=clang++, or leave ZD_JSON_BUILD_FUZZERS "
            "OFF to build the rest of the project with "
            "${CMAKE_CXX_COMPILER_ID}."
        )
    endif()

    set(target_name zd_fuzz_${name})
    set(source_file "${CMAKE_SOURCE_DIR}/tests/fuzz/fuzz_${name}.cpp")
    set(corpus_dir "${CMAKE_SOURCE_DIR}/tests/fuzz/corpus/fuzz_${name}")

    add_executable(${target_name} "${source_file}")
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "zd_fuzz_${name}_run")

    target_link_libraries(${target_name} PRIVATE zd_json::zd_json)

    target_include_directories(${target_name} PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/internal"
    )

    # -g: symbolized crash reports. -O1: fast enough to fuzz at speed
    # while keeping sanitizer stack traces useful (matches upstream
    # libFuzzer guidance over -O0 or -O2/-O3).
    target_compile_options(${target_name} PRIVATE
        -g
        -O1
        -fsanitize=fuzzer,address,undefined
        -fno-sanitize-coverage=stack-depth
        -fno-omit-frame-pointer
    )
    target_link_options(${target_name} PRIVATE
        -fsanitize=fuzzer,address,undefined
        -fno-sanitize-coverage=stack-depth
    )

    if(WIN32)
        get_filename_component(CLANG_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        file(GLOB PROFILE_LIBS "${CLANG_BIN_DIR}/../lib/clang/*/lib/windows/clang_rt.profile*.lib")
        if(PROFILE_LIBS)
            target_link_libraries(${target_name} PRIVATE ${PROFILE_LIBS})
        endif()

        file(GLOB ASAN_DLLS "${CLANG_BIN_DIR}/../lib/clang/*/lib/windows/clang_rt.asan_dynamic*.dll")
        if(ASAN_DLLS)
            list(GET ASAN_DLLS 0 ASAN_DLL)
            add_custom_command(TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${ASAN_DLL}" "$<TARGET_FILE_DIR:${target_name}>/"
            )
        endif()
    endif()

    zd_json_enable_warnings(${target_name})

    if(EXISTS "${corpus_dir}")
        add_test(
            NAME ${target_name}_corpus_replay
            COMMAND ${target_name} -runs=0 "${corpus_dir}"
        )
    endif()
endfunction()
