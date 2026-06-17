| Supported Targets | ESP32 | Linux |
| ----------------- | ----- | ----- |

# Error Codes Registration Check

This test app verifies that all `ESP_ERR_*` error codes defined across ESP-IDF
components are correctly registered via `idf_define_esp_err_codes()` and can be
resolved by `esp_err_to_name()`.

## How it works

1. At CMake configure time, `err_codes_extract.py` scans all ESP-IDF component
   headers to collect every `ESP_ERR_*` define.
2. A test source file is generated that calls `esp_err_to_name()` for each
   collected error code and asserts the result matches the expected name.
3. The app is built and run (e.g., in QEMU or on hardware).

## Building and running

```bash
cd tools/test_apps/build_system/err_codes_check
idf.py set-target esp32
idf.py build
# Run in QEMU or flash to hardware
```
