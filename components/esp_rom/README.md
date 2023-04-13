# `esp_rom` Component

## Function Description

`esp_rom` component contains each chip's ROM functions, which are used in the ROM bootloader, 2nd bootloader, esp_tool flash stub and some driver code (e.g. GPIO matrix). ROM functions as not treated as public APIs, attentions are required when you use ROM functions:

1. ROM functions are **not** thread-safe in RTOS, extra locks are needed to be around the ROM functions.
2. Names/signatures/behaviors of ROM function may be different between chips.
3. ROM functions are not guaranteed to exist across all chips.

When using ROM functions in esp-idf, the including convention is `<target>/rom/<header_file>.h`. This can prevent you from using a nonexistent ROM function for a specific `<target>`. Thus ROM functions are recommended for use in a target-specific source file. For example, `bootloader_esp32.c` can include `esp32/rom/<header_file>.h` without any violations. However, this is not the case when it comes to a common source file that also wants to use some of the ROM functions. The include list would be quite extensive:

```c
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/uart.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/uart.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/uart.h"
...
```

So, we added a wrapper for those commonly used ROM functions. They're declared in `esp_rom/include/esp_rom_xxx.h`. Unlike the original ROM functions, these extracted ones are expected to exist across all chips. If some of them are missed in the new chips, we will implement them again in `esp_rom/patches`. These ROM APIs are always prefixed with the name `esp_rom` (e.g. `esp_rom_printf`), so that it's obvious to know whether a function is linked to ROM.

Most of the time, the ROM wrapper APIs are just alias to the original ROM functions by linker script `esp_rom/<target>/ld/<target>.rom.api.ld`. For example, `esp_rom_printf` is alias to `ets_printf` in the following way:

```
PROVIDE ( esp_rom_printf = ets_printf );
```

If some original ROM functions have changed the behavior or have bugs, we should override them in the wrapper layer. A common example is the `esp_rom_install_uart_printf()`, on ESP32 and ESP32S2, it's just alias to `ets_install_uart_printf`, but on other chips, it's re-implemented in the `esp_rom/patches/esp_rom_uart.c`. To some extent, the ROM wrapper layer works like an anti-corrosion layer between esp-rom project and esp-idf project.

As ROM functions are unique to each target, features are as well. For example, ESP32 has the `tjpgd` library built into the ROM, but ESP32S2 hasn't. We have a header file `esp_rom/<target>/esp_rom_caps.h` declaring the features that are supported by each target. Based on the macros defined there, we can decide whether a function should be patched or whether a feature should be re-implemented.

## Directory Structure

```
.
├── CMakeLists.txt
├── <target/chip_name>
│   ├── esp_rom_caps.h
│   └── ld
│       ├── <target>.rom.api.ld
│       ├── <target>.rom.ld
│       ├── <target>.rom.libgcc.ld
│       ├── <target>.rom.newlib.ld
│       ├── <target>.rom.newlib-nano.ld
│       ├── <target>.rom.version.ld
│       └── ... // other ROM linker scripts, added when bring up new chip
├── include
│   ├── <target/chip_name>
│   │   └── rom
│   │       ├── cache.h
│   │       ├── efuse.h
│   │       ├── esp_flash.h
│   │       ├── ets_sys.h
│   │       ├── gpio.h
│   │       ├── uart.h
│   │       └── ... // other original ROM header files, added when bring up new chip
│   ├── esp_rom_gpio.h
│   ├── esp_rom_md5.h
│   ├── esp_rom_sys.h
│   ├── esp_rom_uart.h
│   └── ... // other ROM wrapper api files
├── Kconfig.projbuild
├── linker.lf
├── patches
│   ├── esp_rom_sys.c
│   ├── esp_rom_uart.c
│   └── ... // other patched source files
├── README.md
└── test
    ├── CMakeLists.txt
    ├── test_miniz.c
    └── ... // other ROM function unit tests
```