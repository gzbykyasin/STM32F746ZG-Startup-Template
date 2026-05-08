# STM32 Nucleo-F746ZG Startup Project

A professional, modular, and power-optimized startup template for the STM32F746ZG (Cortex-M7) microcontroller. This project implements a robust system lifecycle management, deterministic FSM architecture, and dual-mode execution (RTOS and Bare-Metal).

![Nucleo-F746ZG Board](./documentation/nucleo_f746zg_board.avif)

## Core Features
- **Centralized Lifecycle:** State-machine driven transitions (Init, Running, Sleep, Fault).
- **Dual Mode Support:** Seamlessly switch between CMSIS-RTOS V2 (FreeRTOS) and Bare-Metal deployments.
- **Power Optimization:** Integrated FreeRTOS Tickless Idle and manual STOP mode management.
- **Hardware Abstraction:** Clean separation between BSP, Drivers, and Application layers.
- **Professional Documentation:** Minimalist Doxygen standard applied across the entire codebase.

---

## Hardware: Nucleo-F746ZG (MB1137)
The NUCLEO-F746ZG board features an ARM® Cortex®-M7 core with 1MB Flash and 320KB RAM.
- **Connectivity:** Integrated Ethernet, USB OTG, and ST-LINK/V2-1 debugger.
- **User Interface:** 3 User LEDs (Green, Blue, Red) and 1 User Button.
- **Power:** Flexible options via USB VBUS (5V), E5V, or VIN (7V-12V).

---

## Installation & Setup

### 1. Prerequisites
- **CMake:** [Download](https://cmake.org/download/) and add to system PATH.
- **Python:** Required for toolchain and third-party scripts.
- **Serial Terminal:** (e.g., PuTTY, TeraTerm) configured for **115200 baud, 8N1**.

### 2. Toolchain Configuration (Automatic)
Install ARM GCC, Ninja, and OpenOCD into the `toolchain/` folder:
```powershell
python scripts/setup_toolchain.py
```

### 3. Third-Party Libraries (Automatic)
Install required third-party libraries (FreeRTOS, CMSIS, HAL) into the `third_party/` folder:
```powershell
python scripts/setup_third_party.py
```

---

## Building & Flashing

### Build Commands
Use the provided scripts for multi-mode compilation. These scripts automatically handle local paths and environment variables.

#### Windows (PowerShell)
```powershell
# Bare-Metal Development (Debug)
.\scripts\build.bat --development --bare-metal --debug

# RTOS Production (Release)
.\scripts\build.bat --production --rtos --release
```

#### Linux/macOS (Bash)
```bash
# Bare-Metal Development (Debug)
./scripts/build.sh --development --bare-metal --debug

# RTOS Production (Release)
./scripts/build.sh --production --rtos --release
```

### Flashing
1. Connect the board via the **CN1** (ST-LINK) Micro-USB port.
2. Run the flash script from the project root:
   ```powershell
   ./scripts/flash.bat
   ```
   Or for Linux/macOS:
   ```bash
   ./scripts/flash.sh
   ```

---

## Debugging with VS Code

The project includes pre-configured `.vscode` settings for seamless hardware debugging.

1. **Recommended Extension:** Install **[Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug)** (marus25).
2. **Launch:** Press **F5** and select your configuration (`RTOS` or `Bare-metal`). VS Code will build, flash, and halt at `main()` automatically.

---

## Project Architecture

### Folder Structure
- `src/system`: Core OS services, state machine, and lifecycle management.
- `src/api`: OS-agnostic hardware abstraction layer.
- `src/bsp`: Board-specific support (GPIO, UART, ADC, ISR dispatching).
- `src/driver`: Peripheral-specific logic (e.g., Ambient light/LED drivers).
- `src/app`: High-level application tasks and logic.
- `src/target`: Hardware configuration (Linker scripts, Startup files, HAL configuration).
- `src/util`: Utility functions and helpers (Reserved for future use; currently empty).

### Documentation Standard
All public APIs use the Doxygen standard in header files. Source files maintain a minimalist approach by documenting only internal entities and keeping function bodies comment-free for maximum readability.

---
*Created by Yasin GOZUBUYUK*