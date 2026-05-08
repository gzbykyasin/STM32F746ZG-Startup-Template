@echo off
setlocal

:: Default environments
set "BUILD_ENV=development"
set "USE_RTOS=ON"
set "BUILD_TYPE=Debug"

:: Add local ninja to PATH for portability
set "PATH=%~dp0..\toolchain\ninja;%PATH%"

:: Parse arguments
:parse_args
if "%~1"=="" goto after_args
if /i "%~1"=="--development" (
    set "BUILD_ENV=development"
) else if /i "%~1"=="--production" (
    set "BUILD_ENV=production"
) else if /i "%~1"=="--rtos" (
    set "USE_RTOS=ON"
) else if /i "%~1"=="--bare-metal" (
    set "USE_RTOS=OFF"
) else if /i "%~1"=="--debug" (
    set "BUILD_TYPE=Debug"
) else if /i "%~1"=="--release" (
    set "BUILD_TYPE=Release"
) else (
    echo [WARNING] Unknown argument %~1.
)
shift
goto parse_args
:after_args

echo [1/2] Configuring CMake (Env: %BUILD_ENV%, RTOS: %USE_RTOS%, Type: %BUILD_TYPE%)...
cmake -G "Ninja" -B build -S . ^
    -DCMAKE_TOOLCHAIN_FILE=cmake/arm-gcc-toolchain.cmake ^
    -DBUILD_ENV=%BUILD_ENV% ^
    -DSYS_USE_RTOS=%USE_RTOS% ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configuration failed!
    exit /b %ERRORLEVEL%
)

echo [2/2] Building project...
cmake --build build
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed!
    exit /b %ERRORLEVEL%
)

echo [SUCCESS] Build completed successfully.
