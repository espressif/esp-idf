| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Import External C Library Directly (cmakev2)

This example demonstrates importing an external C library that is a pure CMake project. It is downloaded at configure time, built by its own CMake, and linked directly to the IDF app without wrapping it as an IDF component.

## Overview

In Build System v1, integrating third-party CMake libraries required wrapping them as IDF components (see the `import_lib` example). With cmakev2, CMake's `FetchContent` module can be used to fetch and build an external CMake library (lwjson) and link the library to an IDF component using `target_link_libraries`.

This example uses [lwjson](https://github.com/MaJerle/lwjson), a lightweight JSON parser for embedded systems.

## Project Structure

```
import_lib_direct/
├── CMakeLists.txt          # Initialize the IDF project
├── README.md
└── main/
    ├── CMakeLists.txt      # FetchContent for lwjson, register component, link
    └── main.c              # Uses lwjson API
```

## How It Works

1. The **main component's CMakeLists.txt** uses `FetchContent_Declare()` and `FetchContent_MakeAvailable()` to download and build lwjson at configure time. The fetched sources land in `./build/_deps/`.

2. After registering the main omponent with `idf_component_register`, it links the fetched library:
   ```cmake
   target_link_libraries(${COMPONENT_LIB} PUBLIC lwjson)
   ```

3. **main.c** uses the lwjson API (`lwjson_init`, `lwjson_parse`, `lwjson_find`, etc.) to parse a sample JSON string and print device name, cores, features, and specs.

## How to Use

Build and flash the example:

```bash
idf.py set-target <target>
idf.py build
idf.py flash monitor
```

## Expected Output

```
I (275) import_lib_direct: lwjson library imported directly (downloaded, built, linked) without IDF component wrapper
I (275) import_lib_direct: Parsing JSON string...
I (285) import_lib_direct: Device name: ESP32
I (285) import_lib_direct: Number of cores: 2
I (285) import_lib_direct: Features:
I (295) import_lib_direct:   - WiFi
I (295) import_lib_direct:   - Bluetooth
I (295) import_lib_direct:   - GPIO
I (305) import_lib_direct: Specifications:
I (305) import_lib_direct:   Flash: 4MB
I (305) import_lib_direct:   RAM: 520KB
I (315) import_lib_direct: Example complete!
```