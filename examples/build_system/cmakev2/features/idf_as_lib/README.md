| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# ESP-IDF as a Library in a Standard CMake Project

This example demonstrates how to use ESP-IDF as a library inside a standard CMake project. The same source files produce either a plain host executable or an ESP-IDF firmware image depending on how the project is invoked.

## How it works

When `idf.py` is used it sets the `ESP_PLATFORM` CMake variable before invoking CMake. A single project-level CMakeLists.txt checks for this variable: if set, it initialises the IDF build system, bundles the required IDF components into one linkable library via `idf_build_library`, compiles the application sources, links them against that library, and produces the final binary and flash targets. Without `ESP_PLATFORM` the same file produces a minimal host executable with no IDF dependency.

`main.c` is the only source file. All IDF API calls inside it are guarded by `#ifdef ESP_PLATFORM`, so the same file compiles cleanly for both targets. On ESP it defines `app_main`; on the host it defines a plain `main`.

## Project Layout

```
idf_as_lib/
├── CMakeLists.txt              ← Single file: host + ESP build logic
├── main.c                      ← One file, two entry points (host & ESP)
└── sdkconfig                   ← ESP-IDF configuration
```

## How to build the host app

```
cmake -B build-host .
cmake --build build-host
./build-host/idf_as_lib.elf
```

### Expected output

```
Hello from host build
Run with idf.py set-target <target> && idf.py build for ESP build.
```

## How to build the ESP-IDF app

```
idf.py set-target <target>
idf.py build flash monitor
```

### Expected output

```
Hello from ESP-IDF build
idf_lib initialized (IDF as library)
This is esp32 chip with 2 CPU core(s), WiFi/BT/BLE, silicon revision v3.0, 2MB external flash
Minimum free heap size: 303764 bytes
Restarting in 10...
...
Restarting now.
```