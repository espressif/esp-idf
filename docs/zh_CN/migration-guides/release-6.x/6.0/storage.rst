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


``esp_vfs_console``
--------------------

``esp_vfs_console`` 组件已更名为 ``esp_stdio``。该组件没有任何公共函数，因此在一般情况下，无需对代码进行任何更新。如果你曾经因为某些原因在项目中声明了对此组件的依赖，现在可以将其删除，因为新的 ``esp_stdio`` 是一个通用组件，默认会被添加到所有组件中。
