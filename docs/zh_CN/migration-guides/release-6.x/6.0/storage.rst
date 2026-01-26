存储
====

:link_to_translation:`en:[English]`

移除已弃用的 VFS SDMMC API
---------------------------

``esp_vfs_fat_sdmmc_unmount`` 函数已弃用且被移除，请使用 ``esp_vfs_fat_sdcard_unmount`` 代替。


VFS
---

- 位于 ``vfs`` 组件中已弃用的 UART-VFS 函数 ``esp_vfs_dev_uart_*`` 现已删除，请改用 UART 驱动中的 API：``uart_vfs_dev_*``。
- 位于 ``vfs`` 组件中已弃用的 USB-Serial-JTAG-VFS 函数 ``esp_vfs_dev_usb_serial_jtag_*`` 现已删除，请改用 USB-Serial-JTAG 驱动中的 API：``usb_serial_jtag_vfs_*``。
- ``esp_vfs_register_fd_range`` 现被视为私有接口，其函数签名已调整为匹配新的 VFS API 风格。仍依赖此内部辅助函数的项目需包含 ``esp_private/socket.h`` 头文件，并请注意该 API 可能在不另行通知的情况下发生变更。
- 基于 ``esp_vfs_t`` 而非 ``esp_vfs_fs_ops_t`` 的传统 VFS API（如 ``esp_vfs_register``）已弃用，并将在下一个主版本中移除。请迁移至基于 ``esp_vfs_fs_ops_t`` 的新 API。
- TERMIOS 支持现已默认禁用。若应用程序需调用 POSIX ``termios`` API（例如用于 UART 配置的 ``tcsetattr``/``tcgetattr`` 接口），请在 menuconfig 中重新启用 :ref:`CONFIG_VFS_SUPPORT_TERMIOS` 选项。

``esp_vfs_console``
--------------------

``esp_vfs_console`` 组件已更名为 ``esp_stdio``。该组件没有任何公共函数，因此在一般情况下，无需对代码进行任何更新。如果你曾经因为某些原因在项目中声明了对此组件的依赖，现在可以将其删除，因为新的 ``esp_stdio`` 是一个通用组件，默认会被添加到所有组件中。


FATFS
-----

- 动态缓冲区 (:ref:`CONFIG_FATFS_USE_DYN_BUFFERS`) 现已默认启用，以在挂载多个卷时减少静态内存占用；如果倾向于静态分配，可在 menuconfig 中关闭此选项。
- 长文件名支持现已默认使用基于堆的缓冲区 (``CONFIG_FATFS_LFN_HEAP=y``)，因此长于 8.3 格式的文件名可直接使用；若需节省堆内存，可在 menuconfig 中禁用长文件名功能(:ref:`CONFIG_FATFS_LONG_FILENAMES`)。
