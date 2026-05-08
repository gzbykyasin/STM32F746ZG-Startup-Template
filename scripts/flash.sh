#!/bin/bash

# Set directories and file paths
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${PROJECT_DIR}/build"
ELF_FILE="${BUILD_DIR}/src/NUCLEO_F746ZG_Startup.elf"
OPENOCD_CFG="${PROJECT_DIR}/scripts/openocd_stm32f7.cfg"

# Default environment
BUILD_ENV="development"

# Parse arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --development) BUILD_ENV="development"; shift ;;
        --production) BUILD_ENV="production"; shift ;;
        *) echo "[WARNING] Unknown parameter passed: $1. Defaulting to --development"; shift ;;
    esac
done

# Default OpenOCD binary if it's in the system path
OPENOCD_BIN="openocd"

# Check if standalone OpenOCD was downloaded in toolchain directory
LOCAL_OPENOCD="${PROJECT_DIR}/toolchain/openocd/bin/openocd"
if [ -f "${LOCAL_OPENOCD}" ]; then
    OPENOCD_BIN="${LOCAL_OPENOCD}"
elif [ -f "${LOCAL_OPENOCD}.exe" ]; then
    OPENOCD_BIN="${LOCAL_OPENOCD}.exe"
else
    echo "[WARNING] Standalone OpenOCD not found in toolchain."
    echo "Make sure to run 'python scripts/setup_toolchain.py' to download it,"
    echo "or ensure 'openocd' is in your system PATH."
fi

# Ensure the build exists
if [ ! -f "${ELF_FILE}" ]; then
    echo "[ERROR] Firmware not found at ${ELF_FILE}"
    echo "Please build the project first using build.sh"
    exit 1
fi

echo "Flashing ${ELF_FILE} using ${OPENOCD_BIN} in ${BUILD_ENV} mode..."

# Export the ELF file path so the TCL script can read it from the environment
export ELF_FILE_FWD="${ELF_FILE}"
RUN_RAM_TCL="${PROJECT_DIR}/scripts/run_ram.tcl"

if [ "${BUILD_ENV}" = "production" ]; then
    "${OPENOCD_BIN}" -f "${OPENOCD_CFG}" -c "program ${ELF_FILE} verify reset exit"
else
    "${OPENOCD_BIN}" -f "${OPENOCD_CFG}" -f "${RUN_RAM_TCL}"
fi

if [ $? -eq 0 ]; then
    echo "[SUCCESS] Flash successful!"
else
    echo "[ERROR] Flash failed! Check if OpenOCD is correctly installed and your board is connected."
    exit 1
fi
