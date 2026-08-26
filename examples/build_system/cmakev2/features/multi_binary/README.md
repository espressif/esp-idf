| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Multi-Binary Example: Multiple Apps with Different Component Sets

This example demonstrates **Build System v2's** capability to build multiple independent firmware binaries from a single project in one build command. Each binary has its own entry point component with different dependencies.

## Overview

Two independent executables are built simultaneously:

1. **`app1.bin`** - Links `app1_main`, `component1`, `component2`
2. **`app2.bin`** - Links `app2_main`, `component1`, `component2`, `component3`

Each app has its own dedicated entry point component that explicitly declares which components it needs. This avoids complex conditional logic and makes dependencies clear.

## Project Structure

All components live under `components/`. Each `idf_build_executable` call picks exactly the subset it requires.

```
multi_binary/
├── CMakeLists.txt
└── components/
    ├── app1_main/      # Entry point for app1
    │   ├── CMakeLists.txt
    │   └── app1_main.c
    ├── app2_main/      # Entry point for app2
    │   ├── CMakeLists.txt
    │   └── app2_main.c
    ├── component1/     # Shared component
    │   ├── CMakeLists.txt
    │   ├── component1.c
    │   └── component1.h
    ├── component2/     # Shared component
    │   ├── CMakeLists.txt
    │   ├── component2.c
    │   └── component2.h
    └── component3/     # Only linked into app2
        ├── CMakeLists.txt
        ├── component3.c
        └── component3.h
```

## Building

### Build both binaries in a single command:

```bash
cd examples/build_system/cmakev2/features/multi_binary
idf.py set-target <target> 
idf.py build
```

This generates both `app1.bin` and `app2.bin` in the `build/` directory.

## Flashing

Flash app1:
```bash
idf.py app1-flash monitor
```

Flash app2:
```bash
idf.py app2-flash monitor
```

You can switch between apps without rebuilding - just select which one to flash.

### Configuration (menuconfig)

When invoking a custom menuconfig target via `idf.py`, you must pass `--no-hints`. For example, use `idf.py app1-menuconfig --no-hints` (plain `idf.py app1-menuconfig` may not work because `idf.py` redirects stdout by default, which breaks the curses-based menu).

> **Note:** Although multiple binaries can be produced from a single project, each component is evaluated only once using the current configuration. If a different configuration is needed for a component, a new project must be created. You cannot use the same component with different configurations within a single project.

## Expected Output

### app1 Output

```
I (xxx) component1: Hello from component1!
I (xxx) component2: Hello from component2!
```

### app2 Output

```
I (xxx) component1: Hello from component1!
I (xxx) component2: Hello from component2!
I (xxx) component3: Hello from component3!
```