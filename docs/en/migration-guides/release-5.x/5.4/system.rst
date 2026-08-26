System
======

:link_to_translation:`zh_CN:[中文]`

ESP-Common
----------

``__VA_NARG__`` and its related macros have been re-named to avoid naming collisions, use the new name with ``ESP`` pre-fix, e.g. ``ESP_VA_NARG`` instead.

Log
---

- `esp_log_buffer_hex` is deprecated, use `ESP_LOG_BUFFER_HEX` instead.
- `esp_log_buffer_char` is deprecated, use `ESP_LOG_BUFFER_CHAR` instead.
- The default value for ``CONFIG_LOG_COLORS`` is now set to false. Colors are added on the host side by default in IDF Monitor. If you want to enable colors in the log output for other console monitors, set ``CONFIG_LOG_COLORS`` to true in your project configuration. To disable automatic coloring in IDF Monitor, run the following command: ``idf.py monitor --disable-auto-color``.

ESP ROM
---------

- All target-specific header files has been moved from `components/esp_rom/include/{target}/` to `/esp_rom/{target}/include/{target}/`, and `components/esp_rom/CMakeLists.txt` has been modified accordingly. If you encounter an error indicating a missing header file, such as ``fatal error: esp32s3/rom/efuse.h: No such file or directory``, try removing the leading relative path from the header file include command. In your current and future development, when including any header files located in `components/esp_rom` path, directly use the header file name without the chip-specific relative folder path.
- All target-specific `rom/miniz.h` files are removed because they are deprecated.
