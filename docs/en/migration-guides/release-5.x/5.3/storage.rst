Storage
=======

:link_to_translation:`zh_CN:[中文]`

VFS
---

The UART implementation of VFS operators has been moved from `vfs` component to `esp_driver_uart` component.

APIs with `esp_vfs_dev_uart_` prefix are all deprecated, replaced with new APIs in `uart_vfs.h` starting with `uart_vfs_dev_` prefix. Specifically,
- ``esp_vfs_dev_uart_register`` has been renamed to ``uart_vfs_dev_register``
- ``esp_vfs_dev_uart_port_set_rx_line_endings`` has been renamed to ``uart_vfs_dev_port_set_rx_line_endings``
- ``esp_vfs_dev_uart_port_set_tx_line_endings`` has been renamed to ``uart_vfs_dev_port_set_tx_line_endings``
- ``esp_vfs_dev_uart_use_nonblocking`` has been renamed to ``uart_vfs_dev_use_nonblocking``
- ``esp_vfs_dev_uart_use_driver`` has been renamed to ``uart_vfs_dev_use_driver``

For compatibility, `vfs` component still registers `esp_driver_uart` as its private dependency. In other words, you do not need to modify the CMake file of an existing project.
