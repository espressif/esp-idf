ESP-Modbus
==========

:link_to_translation:`en:[English]`

乐鑫的 ESP-Modbus 库 (esp-modbus) 支持基于 RS485、Wi-Fi 和以太网接口的 Modbus 通信。
自 ESP-IDF v5.0 版本以来， 组件 ``freemodbus`` 已被移动到单独的代码仓库中:

* `GitHub 上的 ESP-Modbus 组件 <https://github.com/espressif/esp-modbus>`__

托管文档
--------

相应文档请参阅：

* `ESP-Modbus 文档 <https://docs.espressif.com/projects/esp-modbus>`__

应用示例
--------

以下示例分别介绍了 ESP-Modbus 库的串行端口、TCP 端口的从机和主机实现。

- :example:`protocols/modbus/serial/mb_slave` 演示了如何使用 {IDF_TARGET_NAME} 作为 Modbus 串行从设备，通过 esp-modbus 栈，使外部 Modbus 主机能够使用 Modbus 协议读取和写入设备参数。

- :example:`protocols/modbus/serial/mb_master` 演示了如何在 {IDF_TARGET_NAME} 上使用 esp-modbus 栈端口作为 Modbus 串行主设备，读取和写入 Modbus 网络中从设备的值。

- :example:`protocols/modbus/tcp/mb_tcp_slave` 演示了 esp-modbus TCP 从设备栈端口，允许外部 Modbus 主机通过 Modbus 协议读取和写入设备参数。

- :example:`protocols/modbus/tcp/mb_tcp_master` 演示了如何在 {IDF_TARGET_NAME} 上使用 esp-modbus 栈端口作为 Modbus TCP 主设备，读取和写入 Modbus 网络中从设备的值。

详情请参阅具体示例的 ``README.md`` 。

协议参考
--------

    -  Modbus 组织与规范协议请参阅 `The Modbus Organization <https://modbus.org/specs.php>`_。
