# @file    CMakeLists.txt
# @brief   Source directory CMake configuration.

set(TARGET_NAME ${CMAKE_PROJECT_NAME}.elf)

# -----------------------------------------------------------------------------
# Runtime mode — must be declared before source collection so that
# RTOS-specific files can be excluded from bare-metal builds.
#
# ON  : Full CMSIS-RTOS V2 / FreeRTOS build (sys_rtos, osKernel, osDelay …)
# OFF : Bare-metal build (no RTOS, no cmsis_os2.h dependency)
# -----------------------------------------------------------------------------
option(SYS_USE_RTOS "Build with CMSIS-RTOS V2 / FreeRTOS (ON) or bare-metal (OFF)" ON)

# Find all source files in the subdirectories
file(GLOB_RECURSE SOURCES
    "api/*.c"
    "app/*.c"
    "bsp/*.c"
    "driver/*.c"
    "system/*.c"
    "target/*.c"
    "util/*.c"
)

# Remove deprecated or redundant files


# Add startup file (assembly)
list(APPEND SOURCES "target/startup_stm32f746zgtx.s")

if(SYS_USE_RTOS)
    message(STATUS "Runtime mode: RTOS (CMSIS-RTOS V2 / FreeRTOS)")
else()
    message(STATUS "Runtime mode: bare-metal (no RTOS)")
endif()

# Define executable
add_executable(${TARGET_NAME} ${SOURCES})

# Link against third-party library
target_link_libraries(${TARGET_NAME} PRIVATE third_party_lib)

# Include directories
target_include_directories(${TARGET_NAME} PRIVATE
    api
    app
    bsp
    driver
    system
    target
    util
)

# Apply compiler warnings
set_project_warnings(${TARGET_NAME})

# Propagate the runtime mode flag to all translation units
if(SYS_USE_RTOS)
    target_compile_definitions(${TARGET_NAME} PRIVATE SYS_USE_RTOS)
endif()

# Build Environment Selection (Development=RAM, Production=Flash)
set(BUILD_ENV "development" CACHE STRING "Build environment (development or production)")

if(BUILD_ENV STREQUAL "production")
    message(STATUS "Build environment: PRODUCTION (Flash execution)")
    set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/target/STM32F746ZGTX_FLASH.ld")
else()
    message(STATUS "Build environment: DEVELOPMENT (RAM execution)")
    # Define macros for RAM execution so system_stm32f7xx.c sets the vector table correctly
    target_compile_definitions(${TARGET_NAME} PRIVATE USER_VECT_TAB_ADDRESS VECT_TAB_SRAM)
    set(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/target/STM32F746ZGTX_RAM.ld")
endif()

# Linker script configuration
target_link_options(${TARGET_NAME} PRIVATE
    -T${LINKER_SCRIPT}
)

# Set dependencies to ensure linker script changes trigger a relink
set_target_properties(${TARGET_NAME} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

# Post-build actions: Generate .bin, .hex, and print detailed size
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${TARGET_NAME}> ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET_NAME}> ${CMAKE_PROJECT_NAME}.bin
    COMMAND python ${CMAKE_SOURCE_DIR}/scripts/print_size.py ${CMAKE_SIZE} $<TARGET_FILE:${TARGET_NAME}>
    COMMENT "Generating .hex and .bin files, and printing detailed memory usage..."
)
