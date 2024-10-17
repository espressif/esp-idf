| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |

This test app runs a few FATFS test cases in a read-only FAT partition.

These tests should be possible to run on any ESP development board, not extra hardware is necessary.

The initial FAT image is generated during the build process in [main/CMakeLists.txt](main/CMakeLists.txt):
- `create_test_files` function creates a set of files expected by the test cases
- `fatfs_create_rawflash_image` generates a FAT image from the set of files (via `fatfsgen.py`)

The generated FAT image is flashed into `storage` partition when running `idf.py flash`.

See [../README.md](../README.md) for more information about FATFS test apps.
