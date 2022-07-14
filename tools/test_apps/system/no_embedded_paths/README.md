| Supported Targets | ESP32 | ESP32-C3 | ESP32-S2 |
| ----------------- | ----- | -------- | -------- |

# No Embedded Paths

This test app exists to verify that paths (like __FILE__) are not compiled into
any object files in configurations where this should be avoided.

Configurations where this is relevant include:

* Assertions disabled (doubles up as a build-time check that disabling assertions doesn't lead to any warnings)
* Silent assertions
* CONFIG_COMPILER_HIDE_PATHS_MACROS is set to replace IDF_PATH and project dir with placeholders when expanding `__FILE__`

Not embedding paths reduces the binary size, avoids leaking information about
the compilation environment, and is a necessary step to support reproducible
builds across projects built in different directories.
