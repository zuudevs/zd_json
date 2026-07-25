cmake_minimum_required(VERSION 3.20)

project(
    zd_json
    VERSION 0.1.0
    LANGUAGES C CXX
)

include(cmake/project.cmake)
include(cmake/options.cmake)
include(cmake/output.cmake)

include(cmake/targets.cmake)

include(cmake/compiler-warning.cmake)
include(cmake/optimization.cmake)

include(cmake/clang-format.cmake)
include(cmake/clang-tidy.cmake)

include(cmake/testing.cmake)
include(cmake/benchmark.cmake)

include(cmake/coverage.cmake)
include(cmake/sanitizer.cmake)

message(STATUS "Project Name    : ${PROJECT_NAME}")
message(STATUS "Version         : ${PROJECT_VERSION}")
message(STATUS "Build Type      : ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler        : ${CMAKE_CXX_COMPILER_ID}")