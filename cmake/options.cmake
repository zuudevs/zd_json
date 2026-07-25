set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ZD_JSON_BUILD_TESTS "Build unit tests" ON)
option(ZD_JSON_BUILD_BENCHMARKS "Build benchmarks" ON)
option(ZD_JSON_BUILD_EXAMPLES "Build examples" ON)

option(ZD_JSON_ENABLE_CLANG_FORMAT "Enable clang-format target" ON)
option(ZD_JSON_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
option(ZD_JSON_ENABLE_STATIC_ANALYSIS "Enable static analysis targets" ON)

option(ZD_JSON_ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(ZD_JSON_ENABLE_COVERAGE "Enable coverage" OFF)

option(ZD_JSON_ENABLE_IPO "Enable interprocedural optimization" ON)
option(ZD_JSON_ENABLE_NATIVE_OPTIMIZATION "Enable native CPU optimizations" OFF)
option(ZD_JSON_ENABLE_AVX2 "Enable AVX2 in release" OFF)