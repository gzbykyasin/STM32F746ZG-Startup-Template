# @file    third_party/CMakeLists.txt
# @brief   Third-party dependencies configuration.

add_library(third_party_lib STATIC)

# --- Include Directories ---
target_include_directories(third_party_lib SYSTEM PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS_Core/CMSIS/Core/Include
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS_Core/CMSIS/RTOS2/Include
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS_FreeRTOS/CMSIS/RTOS2/FreeRTOS/Include
    ${CMAKE_CURRENT_LIST_DIR}/CMSIS_Device/Include
    ${CMAKE_CURRENT_LIST_DIR}/STM32F7xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/include
    ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/portable/GCC/ARM_CM7/r0p1
    ${CMAKE_CURRENT_LIST_DIR}/FatFs/source
    ${CMAKE_SOURCE_DIR}/src/system  # Config headers: stm32f7xx_hal_conf.h, FreeRTOSConfig.h, ffconf.h
    ${CMAKE_SOURCE_DIR}/src/target  # Config headers
)

# --- Source Files ---
# STM32F7xx HAL Driver
file(GLOB HAL_SOURCES "${CMAKE_CURRENT_LIST_DIR}/STM32F7xx_HAL_Driver/Src/*.c")
# Exclude template files from HAL
list(FILTER HAL_SOURCES EXCLUDE REGEX ".*_template\\.c$")

# FreeRTOS
file(GLOB FREERTOS_SOURCES "${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/*.c")
set(FREERTOS_PORT_SOURCE "${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/portable/GCC/ARM_CM7/r0p1/port.c")
set(FREERTOS_MEM_SOURCE "${CMAKE_CURRENT_LIST_DIR}/FreeRTOS/portable/MemMang/heap_4.c")
set(CMSIS_RTOS_SOURCE "${CMAKE_CURRENT_LIST_DIR}/CMSIS_FreeRTOS/CMSIS/RTOS2/FreeRTOS/Source/cmsis_os2.c")

# FatFs
file(GLOB FATFS_SOURCES "${CMAKE_CURRENT_LIST_DIR}/FatFs/source/*.c")

# Add all sources to the library
target_sources(third_party_lib PRIVATE
    ${HAL_SOURCES}
    ${FATFS_SOURCES}
)

if(SYS_USE_RTOS)
    target_sources(third_party_lib PRIVATE
        ${FREERTOS_SOURCES}
        ${FREERTOS_PORT_SOURCE}
        ${CMSIS_RTOS_SOURCE}
    )
    # Note: heap_4.c is excluded because we are using 100% static allocation.
endif()

# --- Compile Definitions ---
target_compile_definitions(third_party_lib PUBLIC
    USE_HAL_DRIVER
    STM32F746xx
    CMSIS_device_header="stm32f7xx.h"
)

# --- Compiler Options ---
# Disable warnings for third-party code
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(third_party_lib PRIVATE -w)
endif()