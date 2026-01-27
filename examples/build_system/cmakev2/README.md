# Build System v2 Examples

This directory contains examples demonstrating **ESP-IDF Build System v2** (`cmakev2`). Build System v2 is the next-generation build system for ESP-IDF, offering improved architecture, better CMake integration, and enhanced features. Build System v2 attempts to be backward compatible with Build System v1 applications and components as much as possible. More information about Build System v2 can be found in the ESP-IDF Programming Guide under **API Guides** → **Build System v2**.

> **Note:** Build System v2 is currently a **Technical Preview**.

---

## Getting Started

### Migrating an Existing Project

Change your project's `CMakeLists.txt` from:

```cmake
# Build System v1
cmake_minimum_required(VERSION 3.22)
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

This update is sufficient for most ESP-IDF projects. Components and application code remain unchanged.

### Building Examples

Building any `cmakev2` example follows the standard workflow:

```bash
cd examples/build_system/cmakev2/get-started/<example_name>
idf.py set-target <target>
idf.py build
idf.py flash monitor
```

---

## Get-Started Examples

These examples mirror the ones in `examples/get-started/` but use Build System v2.

### hello_world

[get-started/hello_world/](./get-started/hello_world/)

The simplest ESP-IDF project with `cmakev2`. It prints "Hello World" and demonstrates the minimal project structure required: a `CMakeLists.txt` with `idf_project_default()` and a main component. Start here to understand how a basic v2 project is set up.

---

## Build System Features Examples

These examples demonstrate advanced build system capabilities unique to or enhanced in `cmakev2`. The components used in the examples in this section are registered with `idf_component_register`, which ensures that newly written components remain compatible with Build System v1.

### component_manager

[features/component_manager/](./features/component_manager/)

Shows how to use the ESP Component Registry with `cmakev2`. Declares dependencies in `idf_component.yml`, which are automatically downloads from ESP Component Registry during the build.

### import_lib

[features/import_lib/](./features/import_lib/)

Demonstrates importing third-party CMake libraries using CMake's `ExternalProject_Add()` module. The example downloads and builds [tinyxml2](https://github.com/leethomason/tinyxml2) from GitHub, wraps it as an IDF component, and uses it to parse XML data. This approach is recommended for C++ libraries that use exceptions, as it properly handles ESP-IDF's C++ runtime integration through `PRIV_REQUIRES cxx`. For simpler C libraries, see `import_lib_direct` which demonstrates direct integration without component wrappers.

### import_prebuilt

[features/import_prebuilt/](./features/import_prebuilt/)

Shows how to import pre-compiled static libraries into your project using `add_prebuilt_library()`. The example includes a `prebuilt/` directory containing a component that gets compiled separately, with its output (`libprebuilt.a` and headers) consumed by the main application. Useful for distributing proprietary libraries or speeding up builds with pre-compiled components.

### multi_config

[features/multi_config/](./features/multi_config/)

Demonstrates building multiple configurations of a single application using CMake presets. The example defines development and production presets, each with different `sdkconfig.defaults` files, separate build directories, and conditional source file compilation. Useful for building binaries for different product variants or deployment environments from one codebase.

Build with presets:
```bash
idf.py --preset default build    # Development build
idf.py --preset prod1 build      # Product 1 build
idf.py --preset prod2 build      # Product 2 build
```

### plugins

[features/plugins/](./features/plugins/)

Demonstrates link-time plugin registration using the `WHOLE_ARCHIVE` component property. Plugins register themselves automatically at startup using `__attribute__((constructor))` functions, without requiring explicit function calls from the main application. The example shows both dynamic registration (constructor functions) and static registration (linker sections with `KEEP()`). Useful for building extensible applications where plugins can be added or removed by simply including or excluding components.

### idf_as_lib

[features/idf_as_lib/](./features/idf_as_lib/)

Shows how to use ESP-IDF components as a library in external CMake projects. The example showcases how a non-IDF project can invoke the ESP-IDF build system to create a library which the executable can link against.

---

## cmakev2-Specific Examples

The following examples demonstrate capabilities that are unique to Build System v2 and cannot be achieved with Build System v1.

### conditional_component

[features/conditional_component/](./features/conditional_component/)

Demonstrates conditional component inclusion driven by Kconfig options, with all components written as pure CMake static libraries (no `idf_component_register`).

### multi_binary

[features/multi_binary/](./features/multi_binary/)

Demonstrates building multiple independent firmware binaries from a single project in one build command. Each binary has its own entry point, component dependencies, and flash target.

The project defines two applications (`app1` and `app2`) with shared and distinct components:

Build and flash:
```bash
idf.py build                # Creates both app1.bin and app2.bin
idf.py app1-flash monitor   # Flash and monitor app1
idf.py app2-flash monitor   # Flash and monitor app2
```

This is useful for creating firmware variants with different features, manufacturing test firmware alongside production firmware, or modular applications where different binaries serve different purposes.

### import_lib_direct

[features/import_lib_direct/](./features/import_lib_direct/)

Demonstrates a cmakev2-specific capability of importing external CMake libraries directly at the project level using `FetchContent` without wrapping them as IDF components. The example downloads [lwjson](https://github.com/MaJerle/lwjson), a lightweight JSON parser, and links it directly to the main component using standard CMake `target_link_libraries()`. This pattern works well for pure C libraries and simplifies third-party library integration.

---

## Further Reading

For comprehensive documentation on Build System v2 APIs, migration details, and advanced usage, see the Build System v2 guide in the ESP-IDF Programming Guide under **API Guides** → **Build System v2**.

---

## Contributing

When adding new examples:
1. Use `cmakev2` APIs where applicable
2. Include a README.md explaining the example
3. Test on multiple targets (esp32, esp32c3, esp32s3)
4. Document which `cmakev2` features are demonstrated
