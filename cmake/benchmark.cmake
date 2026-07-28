if(ZD_JSON_BUILD_BENCHMARKS)
    find_package(benchmark CONFIG QUIET)
    if(NOT benchmark_FOUND AND NOT TARGET benchmark::benchmark AND NOT TARGET benchmark)
        include(FetchContent)
        FetchContent_Declare(
            benchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG        v1.9.1
        )
        set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(benchmark)
    endif()

    if(TARGET benchmark AND NOT TARGET benchmark::benchmark)
        add_library(benchmark::benchmark ALIAS benchmark)
    endif()
endif()