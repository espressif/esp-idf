| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# C++ build test

This build-only app can be used to check if certain headers, macros or features can be successfully compiled in a C++ source file. It furthermore checks the current C++ language standard set in IDF is the expected one.

**Note:** C++ compatibility of all public header files is checked by [general public header check](../../../ci/check_public_headers.py) already.

## This App Broke My Build :(

It is possible that the language standard for IDF has been changed without changing the test in this application.

## Adding New Test
Create a new file `main/test_<name>.cpp` and add it to main/CMakeLists.txt. If you need to check specific compiler flags, use `set_source_files_properties` CMake function to adjust the compilation flags for the given source file.
