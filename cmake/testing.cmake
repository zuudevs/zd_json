include(CTest)

if(ZD_JSON_BUILD_TESTS)
    enable_testing()

    find_package(GTest CONFIG QUIET)
    if(NOT GTest_FOUND AND NOT TARGET GTest::gtest AND NOT TARGET gtest)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.15.2
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    foreach(gt_target gtest gtest_main gmock gmock_main)
        if(TARGET ${gt_target})
            if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
                target_compile_options(${gt_target} PRIVATE -Wno-character-conversion -Wno-error)
            endif()
            if(MSVC)
                target_compile_options(${gt_target} PRIVATE /WX-)
            endif()
        endif()
    endforeach()

    if(TARGET gtest AND NOT TARGET GTest::gtest)
        add_library(GTest::gtest ALIAS gtest)
    endif()

    if(TARGET gtest_main AND NOT TARGET GTest::gtest_main)
        add_library(GTest::gtest_main ALIAS gtest_main)
    endif()
endif()