# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Context

This is a fork of [Espressif ESP-IDF](https://github.com/espressif/esp-idf) (the official IoT Development Framework for Espressif SoCs). The fork is maintained at `origin` (`yangshining/esp-idf`) and tracks `upstream` (`espressif/esp-idf`). The active development branch is `adapt/edge-ai-lab`, which tracks `upstream/master`. Adaptation/customization changes should be pushed to `origin adapt/edge-ai-lab`, not to `upstream`.

## Environment Setup

ESP-IDF must be installed and activated before using `idf.py`:

```bash
# One-time installation (specify target chips as needed)
./install.sh esp32 esp32s3

# Every new shell session — must be sourced, not executed
. ./export.sh
```

After sourcing `export.sh`, the `idf.py` command becomes available.

## Building a Project

ESP-IDF itself is not directly buildable — work happens inside example or user projects. Navigate to a project directory first:

```bash
cd examples/get-started/hello_world

idf.py set-target esp32s3          # Set the target chip
idf.py menuconfig                  # Interactive Kconfig configuration
idf.py build                       # Compile app + bootloader + partition table
idf.py -p /dev/ttyUSB0 flash       # Flash to device
idf.py -p /dev/ttyUSB0 monitor     # Serial monitor (exit with Ctrl-])
idf.py -p /dev/ttyUSB0 flash monitor  # Flash and monitor in one step
idf.py app                         # Build app only (skip bootloader/partition table)
idf.py app-flash                   # Flash app only
idf.py erase-flash                 # Erase entire flash
idf.py size                        # Print binary size summary
```

Supported targets: `esp32`, `esp32s2`, `esp32s3`, `esp32c2`, `esp32c3`, `esp32c5`, `esp32c6`, `esp32c61`, `esp32h2`, `esp32p4`. Preview targets: `linux`, `esp32h21`, `esp32h4`, `esp32s31`.

## Running Tests

Tests use `pytest` with the `pytest-embedded` plugin (requires real hardware or QEMU):

```bash
# Run tests for a specific example/component (from project directory)
pytest pytest_*.py -m generic

# Run with a specific target and port
pytest pytest_*.py --target esp32s3 -p /dev/ttyUSB0

# Run host-side unit tests (linux target, no hardware needed)
idf.py set-target linux
idf.py build
./build/<app_name>.elf
```

Test files follow the naming convention `pytest_*.py`. Markers are defined in `pytest.ini` (e.g., `generic`, `qemu`, `eth_*`, `wifi_*`).

## Code Style and Linting

**C/C++ formatting** uses `astyle_py`:
```bash
# Format a C/C++ file
tools/format.sh path/to/file.c
```
Style: OTBS, 4-space indent, 120-char max continuation indent.

**Python linting/formatting** uses `ruff` (configured in `ruff.toml`):
```bash
ruff check --fix .
ruff format .
```
Python style: single quotes, 120-char line length, target Python 3.10+.

**Pre-commit hooks** enforce both (run automatically on `git commit`):
```bash
pre-commit run --all-files   # Run all checks manually
```

## Architecture Overview

### Build System

ESP-IDF uses a **CMake + Ninja** build system wrapped by `idf.py`. The top-level `CMakeLists.txt` is a library file — it cannot be built directly. Each project has its own `CMakeLists.txt` that calls `project()` after including ESP-IDF's project infrastructure.

Key cmake modules in `tools/cmake/`:
- `project.cmake` — main project setup, component discovery, Kconfig integration
- `component.cmake` — component registration macros (`idf_component_register`)
- `build.cmake` — build property management (`idf_build_set_property`, `idf_build_get_property`)
- `kconfig.cmake` — Kconfig → `sdkconfig` → `sdkconfig.h` pipeline

### Component System

All reusable code lives in `components/`. Each component has:
- `CMakeLists.txt` calling `idf_component_register(SRCS ... INCLUDE_DIRS ... REQUIRES ...)`
- `Kconfig` or `Kconfig.projbuild` for configuration options
- `include/` for public headers

Key components:
- `freertos` — RTOS kernel (SMP-capable)
- `esp_system` — system init, panic handler, reset
- `soc` — SoC-specific register definitions and capabilities (no logic)
- `hal` — hardware abstraction layer (thin wrappers over `soc`)
- `driver` / `esp_driver_*` — peripheral drivers (split into individual components per peripheral)
- `esp_wifi`, `esp_netif_stack`, `lwip` — networking stack
- `nvs_flash` — non-volatile storage
- `bootloader`, `bootloader_support` — first/second stage bootloader
- `heap` — multi-heap memory manager with optional poisoning/tracing

### Tools

- `tools/idf.py` — top-level CLI entry point, delegates to `tools/idf_py_actions/`
- `tools/idf_tools.py` — manages toolchain installation
- `tools/idf_monitor.py` — serial monitor with crash decoder
- `tools/cmake/` — ESP-IDF CMake infrastructure

### Kconfig / Configuration

`idf.py menuconfig` generates `sdkconfig` in the project directory. `sdkconfig.rename` in the repo root maps old option names to new ones for backwards compatibility. Component Kconfig files use `CONFIG_` prefixed symbols which become C macros in `sdkconfig.h`.

### Submodules

Several components contain Git submodules (e.g., `mbedtls`, `nimble`, `openthread`, `unity`, `cJSON`). After cloning (especially non-GitHub forks), run:
```bash
tools/set-submodules-to-github.sh   # Fix submodule URLs for non-GitHub forks
git submodule update --init --recursive
```
