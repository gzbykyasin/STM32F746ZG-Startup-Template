@echo off
setlocal

:: Set directories and file paths BEFORE any 'shift' commands!
set "PROJECT_DIR=%~dp0.."
set "BUILD_DIR=%PROJECT_DIR%\build"
set "ELF_FILE=%BUILD_DIR%\src\NUCLEO_F746ZG_Startup.elf"
set "OPENOCD_CFG=%PROJECT_DIR%\scripts\openocd_stm32f7.cfg"

:: Default environment
set "BUILD_ENV=development"

:: Parse arguments
:parse_args
if "%~1"=="" goto after_args
if /i "%~1"=="--development" (
    set "BUILD_ENV=development"
) else if /i "%~1"=="--production" (
    set "BUILD_ENV=production"
) else (
    echo [WARNING] Unknown argument %~1. Defaulting to --development.
)
shift
goto parse_args
:after_args

:: Default OpenOCD binary if it's in the system path
set "OPENOCD_BIN=openocd"

:: Check if standalone OpenOCD was downloaded in toolchain directory
set "LOCAL_OPENOCD=%PROJECT_DIR%\toolchain\openocd\bin\openocd.exe"
if exist "%LOCAL_OPENOCD%" (
    set "OPENOCD_BIN=%LOCAL_OPENOCD%"
) else (
    echo [WARNING] Standalone OpenOCD not found in toolchain.
    echo Make sure to run 'python scripts\setup_toolchain.py' to download it,
    echo or ensure 'openocd' is in your system PATH.
)

:: Ensure the build exists
if not exist "%ELF_FILE%" (
    echo [ERROR] Firmware not found at %ELF_FILE%
    echo Please build the project first using build.bat
    exit /b 1
)

echo Flashing %ELF_FILE% using %OPENOCD_BIN% in %BUILD_ENV% mode...

:: OpenOCD interprets backslashes as escape characters, so we must replace them with forward slashes
set "ELF_FILE_FWD=%ELF_FILE:\=/%"
set "OPENOCD_CFG_FWD=%OPENOCD_CFG:\=/%"
set "RUN_RAM_TCL=%PROJECT_DIR%\scripts\run_ram.tcl"
set "RUN_RAM_TCL_FWD=%RUN_RAM_TCL:\=/%"

if "%BUILD_ENV%"=="production" (
    "%OPENOCD_BIN%" -f "%OPENOCD_CFG_FWD%" -c "program \"%ELF_FILE_FWD%\" verify reset exit"
) else (
    "%OPENOCD_BIN%" -f "%OPENOCD_CFG_FWD%" -f "%RUN_RAM_TCL_FWD%"
)

if %ERRORLEVEL% equ 0 (
    echo [SUCCESS] Flash successful!
) else (
    echo [ERROR] Flash failed! Check if OpenOCD is correctly installed and your board is connected.
    exit /b %ERRORLEVEL%
)
