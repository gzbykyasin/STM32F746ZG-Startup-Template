# @file    compiler_warnings.cmake
# @brief   Defines strict compiler warnings for the project to align with MISRA-C 
#          and ensure code quality.

# @brief Sets strict compiler warnings for a given CMake target.
# 
# This function appends warning flags like -Wall, -Wextra, -Wpedantic,
# and others to the specified target. These flags help detect potential
# bugs and enforce better coding practices.
# 
# @param target The CMake target to apply warnings to.
function(set_project_warnings target)
    set(WARNING_FLAGS
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wcast-align
        -Wdouble-promotion
        -Wformat=2
        -Wundef
    )

    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE ${WARNING_FLAGS})
    endif()
endfunction()
