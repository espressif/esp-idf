Storage
=======

:link_to_translation:`zh_CN:[中文]`

Removed VFS SDMMC Deprecated API
--------------------------------

Deprecated ``esp_vfs_fat_sdmmc_unmount`` function is now removed. Please use ``esp_vfs_fat_sdcard_unmount`` instead.


VFS
---

- Deleted deprecated UART-VFS functions (``esp_vfs_dev_uart_*``) located in the ``vfs`` component. Please use API from UART driver instead: ``uart_vfs_dev_*``.
- Deleted deprecated USB-Serial-JTAG-VFS functions (``esp_vfs_dev_usb_serial_jtag_*``) located in the ``vfs`` component. Please use API from USB-Serial-JTAG driver instead: ``usb_serial_jtag_vfs_*``.


``esp_vfs_console``
--------------------

The ``esp_vfs_console`` component has been renamed to ``esp_stdio``. This component did not have any public functions, so in normal circumstances, there is no need to update your code. If you for any reason had declared a dependency on this component, it can be deleted, as the new ``esp_stdio`` is a common component that is added to all components by default.
