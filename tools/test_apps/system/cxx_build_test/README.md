| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# C++ build test

This build-only app can be used to check if certain headers, macros or features can be successfully compiled in a C++ source file. It furthermore checks the current C++ language standard set in IDF is the expected one.

## This App Broke My Build :(

It is likely that one of the following situations occurred:
1. The language standard for IDF has been changed without changing the test in this application.
2. Code incompatible to C++ has been added to one of the public header files in IDF. Check the C++ documentation section [Combining C and C++ code](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/cplusplus.html#combining-c-and-c-code) for more details.

## Adding New Test
Create a new file `main/test_<name>.cpp` and add it to main/CMakeLists.txt. If you need to check specific compiler flags, use `set_source_files_properties` CMake function to adjust the compilation flags for the given source file.
