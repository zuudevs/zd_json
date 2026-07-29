include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_ERROR)

function(zd_json_enable_release_optimizations target_name)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target_name} PRIVATE
            $<$<CONFIG:Release>:-O3>
            $<$<CONFIG:Release>:-ffunction-sections>
            $<$<CONFIG:Release>:-fdata-sections>
        )

        if(ZD_JSON_ENABLE_NATIVE_OPTIMIZATION)
            target_compile_options(${target_name} PRIVATE
                $<$<CONFIG:Release>:-march=native>
                $<$<CONFIG:Release>:-mtune=native>
            )
        endif()

        if(ZD_JSON_ENABLE_AVX2)
            target_compile_options(${target_name} PRIVATE
                $<$<CONFIG:Release>:-mavx2>
            )
        endif()

        if(WIN32)
            target_link_options(${target_name} PRIVATE
                $<$<CONFIG:Release>:LINKER:/OPT:REF>
                $<$<CONFIG:Release>:LINKER:/OPT:ICF>
            )
        else()
            target_link_options(${target_name} PRIVATE
                $<$<CONFIG:Release>:LINKER:--gc-sections>
            )
        endif()

    elseif(MSVC)
        target_compile_options(${target_name} PRIVATE
            $<$<CONFIG:Release>:/O2>
            $<$<CONFIG:Release>:/GL>
            $<$<CONFIG:Release>:/arch:AVX2>
        )
        target_link_options(${target_name} PRIVATE
            $<$<CONFIG:Release>:/LTCG>
        )
    endif()

    if(IPO_SUPPORTED AND ZD_JSON_ENABLE_IPO AND NOT ZD_JSON_BUILD_FUZZERS)
        set_property(TARGET ${target_name} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    endif()
endfunction()