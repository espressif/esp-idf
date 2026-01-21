# Build System v2 Examples

This directory contains examples demonstrating **ESP-IDF Build System v2** (`cmakev2`). Build System v2 is the next-generation build system for ESP-IDF, offering improved architecture, better CMake integration, and enhanced features. `cmakev2` is backward compatible with projects and components written for Build System v1. The examples in this folder give a preview of how to work with the new build system.

> **Note:** Build System v2 is currently a **Technical Preview**. For comprehensive documentation, see the [Build System v2 Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system-v2.html).

---

## Quick Start

### Migrating an Existing Project

Change your project's `CMakeLists.txt` from:

```cmake
# Build System v1
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(my_project)
```

To:

```cmake
# Build System v2
cmake_minimum_required(VERSION 3.22)
include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
project(my_project C CXX ASM)
idf_project_default()
```

Note: This update should be sufficient for most ESP-IDF projects. Everything else remains unchanged.

### Building any example

Building a `cmakev2` example does not involve any new steps.

```bash
cd examples/build_system/cmakev2/<example_name>
idf.py set-target <target>
idf.py build
idf.py flash monitor
```

---

### Examples Overview

### Get-Started Examples

These examples preview basic ESP-IDF features to get started. These examples are identical to the ones in `./examples/get_started/` but updated to use `cmakev2`:

#### hello_world
**Location:** [get-started/hello_world/](./get-started/hello_world/)

The simplest ESP-IDF project with `cmakev2`. Start here to understand the basic structure.

```cmake
cmake_minimum_required(VERSION 3.22)
include($ENV{IDF_PATH}/tools/cmakev2/idf.cmake)
project(hello_world C CXX ASM)
idf_project_default()
```

**Key Concepts:** `idf_project_default()`, basic component registration

---

### blink
**Location:** [get-started/blink/](./get-started/blink/)

LED blinking example with Kconfig configuration and Component Manager integration.

**Key Concepts:** Kconfig options, Component Manager with `cmakev2` 

---

## Build System Features Examples

These examples demonstrate the build system capabilities and some `cmakev2` specific features.

## Build System v2 API Quick Reference

### Project Setup

| Function | Use Case |
|----------|----------|
| `idf_project_default()` | Simple projects - does everything automatically |
| `idf_project_init()` | Advanced control - manual component/executable setup |

### Build Functions

| Function | Description |
|----------|-------------|
| `idf_build_executable(name ...)` | Create an ELF executable |
| `idf_build_binary(exe ...)` | Generate .bin from executable |
| `idf_build_library(name ...)` | Create unified library interface |
| `idf_flash_binary(bin ...)` | Create flash target |
| `idf_sign_binary(bin ...)` | Sign binary for secure boot |
| `idf_component_include(name)` | Explicitly include a component |

### Utility Functions

| Function | Description |
|----------|-------------|
| `idf_build_generate_metadata(bin)` | Generate project_description.json |
| `idf_check_binary_size(bin)` | Verify binary fits in partition |
| `idf_create_menuconfig(exe ...)` | Create menuconfig target |

---

## Key Differences from Build System v1

| Aspect | Build System v1 | Build System v2 |
|--------|-----------------|-----------------|
| **Include** | `tools/cmake/project.cmake` | `tools/cmakev2/idf.cmake` |
| **CMake Version** | 3.16+ | 3.22+ |
| **Project Call** | `project(name)` | `project(name C CXX ASM)` |
| **Initialization** | Implicit | `idf_project_default()` or `idf_project_init()` |

---

## Migration Checklist

- [ ] Update `cmake_minimum_required(VERSION 3.22)`
- [ ] Change include to `$ENV{IDF_PATH}/tools/cmakev2/idf.cmake`
- [ ] Add languages to project(): `project(name C CXX ASM)`
- [ ] Add `idf_project_default()` or `idf_project_init()` after project()

---

## Troubleshooting

### Common Issues

**"CMake version too old"**
- Build System v2 requires CMake 3.22+. Update your CMake installation.

**"Component validation warnings"**
- These are informational. They help identify potential dependency issues. Address by adding proper `idf_component_include()` calls and updated `REQUIRES`/`PRIV_REQUIRES` list.

**"Binary too large for partition"**
- `cmakev2` evaluates components and associated Kconfig options early, leading to some components getting included in the build which were not part of the binary in Build System v1. This is being optimized and it is encouraged to actively include only required components in the build. If you have specific observation regarding, do report to us for further investigation.

---

## Contributing

When adding new examples:
1. Use `cmakev2` APIs where applicable
2. Include a README.md explaining the example
3. Test on multiple targets (esp32, esp32c3, esp32s3)
4. Document which `cmakev2` features are demonstrated
