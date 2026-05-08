# @file    arm-gcc-toolchain.cmake
# @brief   CMake toolchain file for cross-compiling with arm-none-eabi-gcc.
#          Target: STM32F746ZG (Cortex-M7, FPU).

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Prevent CMake from testing the compiler during initialization, 
# which often fails in bare-metal cross-compilation environments.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Add local toolchain directory to search path
set(LOCAL_TOOLCHAIN_DIR "${CMAKE_CURRENT_LIST_DIR}/../toolchain/arm-gnu-toolchain/bin")
if(EXISTS ${LOCAL_TOOLCHAIN_DIR})
    list(APPEND CMAKE_PROGRAM_PATH ${LOCAL_TOOLCHAIN_DIR})
endif()

# Define toolchain paths
find_program(CMAKE_C_COMPILER arm-none-eabi-gcc REQUIRED)
find_program(CMAKE_CXX_COMPILER arm-none-eabi-g++ REQUIRED)
find_program(CMAKE_ASM_COMPILER arm-none-eabi-gcc REQUIRED)
find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(CMAKE_SIZE arm-none-eabi-size REQUIRED)

# Architecture specific flags for STM32F746ZG
# STM32F746xx: Cortex-M7, with single precision FPU
set(MCU_FLAGS "-mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16")

# Set C Compiler Flags
set(CMAKE_C_FLAGS "${MCU_FLAGS} -ffunction-sections -fdata-sections" CACHE INTERNAL "C Compiler flags")
set(CMAKE_CXX_FLAGS "${MCU_FLAGS} -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti" CACHE INTERNAL "CXX Compiler flags")
set(CMAKE_ASM_FLAGS "${MCU_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "ASM Compiler flags")

# Build-type specific flags
set(CMAKE_C_FLAGS_DEBUG "-Og -g3" CACHE INTERNAL "C Debug flags")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g3" CACHE INTERNAL "CXX Debug flags")
set(CMAKE_ASM_FLAGS_DEBUG "-g3" CACHE INTERNAL "ASM Debug flags")

set(CMAKE_C_FLAGS_RELEASE "-Os -DNDEBUG" CACHE INTERNAL "C Release flags")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG" CACHE INTERNAL "CXX Release flags")

# Set Linker Flags
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -Wl,--gc-sections -Wl,-Map=${CMAKE_PROJECT_NAME}.map --specs=nano.specs" CACHE INTERNAL "Linker flags")
