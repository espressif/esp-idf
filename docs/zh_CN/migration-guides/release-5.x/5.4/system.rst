系统
======

:link_to_translation:`en:[English]`

日志
------

- `esp_log_buffer_hex` 已弃用，应使用 `ESP_LOG_BUFFER_HEX` 替代。
- `esp_log_buffer_char` 已弃用，应使用 `ESP_LOG_BUFFER_CHAR` 替代。

ESP ROM
---------

- 针对特定目标的头文件已从 `components/esp_rom/include/{target}/` 移至 `/esp_rom/{target}/include/{target}/`，`components/esp_rom/CMakeLists.txt` 文件已相应更新。如果遇到报错提示缺少头文件，例如 ``fatal error: esp32s3/rom/efuse.h: No such file or directory``，请在头文件包含命令中删除前导的相对路径。在今后的开发中，如果需要包含 `components/esp_rom` 路径下的任何头文件，请直接使用头文件名，无需包含特定芯片的相对文件夹路径。
- 所有特定目标的 `rom/miniz.h` 文件已移除，因为它们已经被弃用。
