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
- ``esp_vfs_register_fd_range`` is now considered private and its signature was changed to match the new VFS API style. Projects that still rely on this internal helper must include ``esp_private/socket.h`` and should be aware that the API may change without notice.
- Legacy VFS APIs (such as ``esp_vfs_register``) that operate on ``esp_vfs_t`` instead of ``esp_vfs_fs_ops_t`` are deprecated and will be removed in the next major release. Switch to the new ``esp_vfs_fs_ops_t``-based APIs.
- TERMIOS support is now disabled by default. Re-enable :ref:`CONFIG_VFS_SUPPORT_TERMIOS` in menuconfig if your application calls POSIX ``termios`` APIs, such as ``tcsetattr``/``tcgetattr`` for UART configuration.


``esp_vfs_console``
--------------------

The ``esp_vfs_console`` component has been renamed to ``esp_stdio``. This component did not have any public functions, so in normal circumstances, there is no need to update your code. If you for any reason had declared a dependency on this component, it can be deleted, as the new ``esp_stdio`` is a common component that is added to all components by default.

FATFS
-----

- Dynamic buffers (:ref:`CONFIG_FATFS_USE_DYN_BUFFERS`) now default to enabled to trim static memory usage when multiple volumes are mounted; turn this off in menuconfig if you prefer static allocation.
- Long filename support now defaults to heap-based buffers (``CONFIG_FATFS_LFN_HEAP=y``) so filenames longer than 8.3 work out of the box; you can disable it in menuconfig (:ref:`CONFIG_FATFS_LONG_FILENAMES`) if you need to conserve heap.
