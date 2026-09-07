| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# LLVM memcpy optimization

Benchmark for the ESP-IDF LLVM optimization framework: `menuconfig` enables
the memcpy optimization, and CMake marks which sources receive it.

This example compares two source files in the same component implementing the
same 16-byte aligned, fixed 1024-byte `memcpy`:

- `memcpy_baseline.c`: built without LLVM optimization.
- `memcpy_optimized.c`: selected with
  `idf_component_enable_llvm_opt(SRCS "memcpy_optimized.c")`.

The project uses performance optimization. When **Compiler options > LLVM
optimizations > Optimize memcpy with PIE** is enabled, ESP-IDF injects the
framework flags only into `memcpy_optimized.c`.

Use the Espressif Clang toolchain (`IDF_TOOLCHAIN=clang`) for the memcpy
speedup. With the default GCC toolchain the example still **builds and runs**
(so CI can board-test correctness on ESP32-P4): the LLVM opt-in markers are
ignored and CMake prints a warning with the Clang commands below. Cycle counts
under GCC are not a meaningful “optimized vs baseline” comparison.

Both ESP32-P4 ECO4 (PIE 2.1) and ECO5+ (PIE 2.2) are supported. The framework
picks the PIE feature from the chip revision selected in menuconfig (IDF
default is ECO5+ / rev >= 3.0). For an ECO4 board, set the revision under
**Component config > ESP System Settings** (or equivalent) to a rev < 3.0
option so `CONFIG_ESP32P4_SELECTS_REV_LESS_V3` is enabled. No example-specific
revision pin is required.

## Build and run

```bash
cd "$IDF_PATH/examples/system/llvm_memcpy_opt"
IDF_TOOLCHAIN=clang idf.py set-target esp32p4
IDF_TOOLCHAIN=clang idf.py build flash monitor
```

`sdkconfig.defaults` enables the menuconfig option for this example. It can
also be changed with:

```bash
IDF_TOOLCHAIN=clang idf.py menuconfig
```

The example verifies both copies against the source, then prints minimum cycle
counts from 512 runs (benchmark helper placed in IRAM). Example ECO4 / PIE 2.1
output:

```text
LLVM memcpy optimization: enabled
Copy: 1024 bytes, source/destination alignment: 16 bytes
Baseline cycles:  671
Optimized cycles: 136
Result: PASS
```

Cycle counts depend on chip revision, clock configuration, toolchain version,
and memory placement. Use values printed by the target as benchmark results.
