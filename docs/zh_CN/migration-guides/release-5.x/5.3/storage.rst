存储
=======

:link_to_translation:`en:[English]`

VFS
---

VFS 操作符的 UART 具体实现函数从 `vfs` 组件挪到了 `esp_driver_uart` 组件中。

所有以 `esp_vfs_dev_uart_` 前缀开头的 API 已被弃用, 更新成在 `uart_vfs.h` 文件中定义的以 `uart_vfs_dev_` 为前缀的一组 API。具体来说，
- ``esp_vfs_dev_uart_register`` 更名为 ``uart_vfs_dev_register``
- ``esp_vfs_dev_uart_port_set_rx_line_endings`` 更名为 ``uart_vfs_dev_port_set_rx_line_endings``
- ``esp_vfs_dev_uart_port_set_tx_line_endings`` 更名为 ``uart_vfs_dev_port_set_tx_line_endings``
- ``esp_vfs_dev_uart_use_nonblocking`` 更名为 ``uart_vfs_dev_use_nonblocking``
- ``esp_vfs_dev_uart_use_driver`` 更名为 ``uart_vfs_dev_use_driver``

为了兼容性，`vfs` 组件依旧将 `esp_driver_uart` 注册成了其私有依赖。换句话说，你无需修改既有项目的 CMake 文件。
