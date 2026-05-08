#!/bin/bash

# Default environments
BUILD_ENV="development"
USE_RTOS="ON"
BUILD_TYPE="Debug"

# Parse arguments
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --development) BUILD_ENV="development" ;;
        --production) BUILD_ENV="production" ;;
        --rtos) USE_RTOS="ON" ;;
        --bare-metal) USE_RTOS="OFF" ;;
        --debug) BUILD_TYPE="Debug" ;;
        --release) BUILD_TYPE="Release" ;;
        *) echo "[WARNING] Unknown argument $1." ;;
    esac
    shift
done

echo "[1/2] Configuring CMake (Env: $BUILD_ENV, RTOS: $USE_RTOS, Type: $BUILD_TYPE)..."
cmake -G "Ninja" -B build -S . \
    -DCMAKE_TOOLCHAIN_FILE=cmake/arm-gcc-toolchain.cmake \
    -DBUILD_ENV=$BUILD_ENV \
    -DSYS_USE_RTOS=$USE_RTOS \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE

if [ $? -ne 0 ]; then
    echo "[ERROR] CMake configuration failed!"
    exit 1
fi

echo "[2/2] Building project..."
cmake --build build
if [ $? -ne 0 ]; then
    echo "[ERROR] Build failed!"
    exit 1
fi

echo "[SUCCESS] Build completed successfully."
