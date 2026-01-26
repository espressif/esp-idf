| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Conditional Component Inclusion Example

This example demonstrates how to conditionally include components at build time using Build System v2's pure CMake approach. Components are included or excluded based on Kconfig configuration options, and all components are written using standard CMake functions rather than IDF-specific abstractions.

## Overview

In Build System v2, components can be written as plain CMake static libraries using `add_library`, `target_link_libraries`, and `target_include_directories`. A component is brought into the build on demand by calling `idf_component_include()`, which sets up the component's target name and invokes its CMakeLists.txt. Dependencies on other components are expressed using the `idf::<name>` alias targets.

This example uses that mechanism to conditionally pull in two utility components — `logging_util` and `math_util` — from within the `main` component, based on Kconfig options.

## Project Structure

The project is initialized manually with `idf_project_init()` and `idf_build_executable()`, which gives precise control over which components are seeded into the build. The `main` component itself is a pure CMake static library. It conditionally includes `logging_util` and/or `math_util` at configure time by calling `idf_component_include()` inside `if(CONFIG_...)` guards and linking the resulting targets with `target_link_libraries`.

`logging_util` depends on IDF's `log` component for `ESP_LOG*` macros. It pulls `log` into the build with `idf_component_include(log)` and links it via the `idf::log` alias target. `math_util` has no IDF dependencies and is a self-contained static library.

## Configuration Options

Two Kconfig options control which components are included:

- `CONFIG_EXAMPLE_ENABLE_LOGGING` — includes and links the `logging_util` component.
- `CONFIG_EXAMPLE_ENABLE_MATH` — includes and links the `math_util` component.

Both default to enabled. Toggle them via menuconfig under **Conditional Component Example Configuration**.

## Building

```bash
cd examples/build_system/cmakev2/conditional_component
idf.py set-target <target>
idf.py build
```

To change which components are included, run `idf.py menuconfig` before rebuilding.

## Output

With both components enabled the serial output shows info and warning log messages from `logging_util` and the results of add, subtract, and multiply operations from `math_util`. Disabling either option removes the corresponding section and replaces it with a "DISABLED" message.
