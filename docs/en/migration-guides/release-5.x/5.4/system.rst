System
======

:link_to_translation:`zh_CN:[中文]`

Log
---

- `esp_log_buffer_hex` is deprecated, use `ESP_LOG_BUFFER_HEX` instead.
- `esp_log_buffer_char` is deprecated, use `ESP_LOG_BUFFER_CHAR` instead.

ESP ROM
---------

- All target-specific header files has been moved from `components/esp_rom/include/{target}/` to `/esp_rom/{target}/include/{target}/`, and `components/esp_rom/CMakeLists.txt` has been modified accordingly. If you encounter an error indicating a missing header file, such as ``fatal error: esp32s3/rom/efuse.h: No such file or directory``, try removing the leading relative path from the header file include command. In your current and future development, when including any header files located in `components/esp_rom` path, directly use the header file name without the chip-specific relative folder path.
- All target-specific `rom/miniz.h` files are removed because they are deprecated.
