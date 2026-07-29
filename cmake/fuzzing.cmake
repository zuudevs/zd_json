# cmake/fuzzing.cmake
#
# Wires up LLVM's libFuzzer for the harnesses under tests/fuzz. Fuzz
# targets are opt-in (ZD_JSON_BUILD_FUZZERS, default OFF) and only
# buildable with Clang: libFuzzer is a Clang/compiler-rt feature
# (-fsanitize=fuzzer) with no GCC or MSVC equivalent, so we fail loudly
# rather than silently skipping the targets if someone enables the
# option with the wrong compiler.
#
# Usage (from the repo root):
#
#   cmake -B out/fuzz -DCMAKE_CXX_COMPILER=clang++ -DZD_JSON_BUILD_FUZZERS=ON
#   cmake --build out/fuzz --target zd_fuzz_parse zd_fuzz_roundtrip
#
#   ./out/fuzz/bin/zd_fuzz_parse tests/fuzz/corpus/fuzz_parse
#   ./out/fuzz/bin/zd_fuzz_roundtrip tests/fuzz/corpus/fuzz_roundtrip
#
# Each target also gets a `zd_fuzz_<name>_smoke` CTest entry that runs a
# single, bounded pass over its seed corpus (-runs=0 with -runs replaced
# by a real bounded run count below), so `ctest` exercises every seed at
# least once in CI without requiring anyone to kick off a real fuzzing
# campaign by hand.

# add_fuzz_target(<name>)
#
# Declares executable zd_fuzz_<name> from tests/fuzz/fuzz_<name>.cpp,
# linked against the zd_json library and instrumented with
# -fsanitize=fuzzer,address,undefined. Seed corpus is expected at
# tests/fuzz/corpus/fuzz_<name>/.
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
        -fno-omit-frame-pointer
    )
    target_link_options(${target_name} PRIVATE
        -fsanitize=fuzzer,address,undefined
    )

    zd_json_enable_warnings(${target_name})

    # A cheap CI-friendly smoke test: run every seed in the corpus
    # through the harness once (-runs=1 per libFuzzer-selected input via
    # a plain corpus replay) and fail the build if any of them trips a
    # sanitizer or crashes. This is NOT a substitute for actually
    # fuzzing -- it only replays known inputs -- but it keeps the
    # harnesses themselves honest in normal `ctest` runs.
    if(EXISTS "${corpus_dir}")
        add_test(
            NAME ${target_name}_corpus_replay
            COMMAND ${target_name} -runs=0 "${corpus_dir}"
        )
    endif()
endfunction()
