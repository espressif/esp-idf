| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |

# C++ build test

This build-only app can be used to check if certain headers, macros or features can be successfully compiled in a C++ source file.

To add a new test, create a new file `main/test_<name>.cpp` and add it to main/CMakeLists.txt.

If you need to check specific compiler flags, use `set_source_files_properties` CMake function to adjust the compilation flags for the given source file.
