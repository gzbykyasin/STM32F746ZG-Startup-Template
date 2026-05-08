# @file    CMakeLists.txt
# @brief   Root CMake configuration file for NUCLEO-F746ZG Startup.

cmake_minimum_required(VERSION 3.22)

# Set the toolchain file before defining the project
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/arm-gcc-toolchain.cmake)

project(NUCLEO_F746ZG_Startup C CXX ASM)

# Add custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(compiler_warnings)

# Export compile commands for tooling (clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Subdirectories
add_subdirectory(third_party)
add_subdirectory(src)
add_subdirectory(tests)
