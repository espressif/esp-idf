Storage
=======

Removed VFS SDMMC Deprecated API
--------------------------------

Deprecated ``esp_vfs_fat_sdmmc_unmount`` function is now removed. Please use ``esp_vfs_fat_sdcard_unmount`` instead.

VFS
---
- Deleted deprecated UART-VFS functions (```esp_vfs_dev_uart_*``) located in the ``vfs`` component. Please use API from UART driver instead: ``uart_vfs_dev_*``.
- Deleted deprecated USB-Serial-JTAG-VFS functions (```esp_vfs_dev_usb_serial_jtag_*``) located in the ``vfs`` component. Please use API from USB-Serial-JTAG driver instead: ``usb_serial_jtag_vfs_*``.

