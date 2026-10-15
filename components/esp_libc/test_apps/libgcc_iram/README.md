| Supported Targets | ESP32-H4 |
| ----------------- | -------- |

This app checks libgcc and compiler-rt placement on targets whose ROM does not provide those helpers.

64-bit integer helpers stay in internal RAM. On GCC, `__clz_tab` stays in DRAM with them. `CONFIG_LIBC_LIBGCC_IN_IRAM` also places the remaining helpers, including software double-precision code, in internal RAM. The build fails when the linked ELF does not match that placement.
