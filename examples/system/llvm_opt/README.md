| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# LLVM optimization (component scope)

Minimal example for the ESP-IDF LLVM optimization framework. It shows how to
apply the LLVM optimizations selected in menuconfig to **an entire component**
with the `ENABLE_LLVM_OPT` argument of `idf_component_register`:

```cmake
idf_component_register(SRCS "llvm_opt_main.c"
                       INCLUDE_DIRS ""
                       ENABLE_LLVM_OPT)
```

Enable the option in menuconfig: **Compiler options > LLVM optimizations >
Optimize memcpy with PIE**. On ESP32-P4 with the Clang toolchain, ESP-IDF then
injects the corresponding compiler flags into every source file of this
component.

With the default GCC toolchain the example still builds (for CI board smoke
tests); `ENABLE_LLVM_OPT` is ignored and CMake prints a warning. Use Clang for
real LLVM opt flags:


## Build and run

Activate the Clang toolchain before configuring the project:

```bash
cd "$IDF_PATH/examples/system/llvm_opt"
IDF_TOOLCHAIN=clang idf.py set-target esp32p4
IDF_TOOLCHAIN=clang idf.py build flash monitor
```

`CONFIG_COMPILER_LLVM_MEMCPY_OPTIMIZATION` is enabled by `sdkconfig.defaults`
and can be changed with `idf.py menuconfig`.

## Related example

For the file-level API (`idf_component_enable_llvm_opt`) and a `memcpy`
benchmark comparing optimized and baseline sources, see the
`llvm_memcpy_opt` example.
