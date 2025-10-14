协议
====

:link_to_translation:`en:[English]`

ESP-TLS
-------

**已移除的废弃 API**

已移除废弃函数 :cpp:func:`esp_tls_conn_http_new`。请使用以下替代函数：

- :cpp:func:`esp_tls_conn_http_new_sync` 用于阻塞连接
- :cpp:func:`esp_tls_conn_http_new_async` 用于非阻塞连接

新 API 需要您使用 :cpp:func:`esp_tls_init` 创建 :cpp:type:`esp_tls_t` 结构，并提供对连接过程的更好控制。

ESP-Modbus
----------

乐鑫ESP-Modbus 库 (esp-modbus) 支持基于 RS485、Wi-Fi 和以太网接口网络的 Modbus 通信。

``esp-modbus v2 (v2.x.x)`` 组件是当前支持的组件版本：

* `GitHub 上的 ESP-Modbus 组件 <https://github.com/espressif/esp-modbus/tree/main>`__

文档
~~~~

* `ESP-MODBUS 稳定版文档 v2.x.x <https://docs.espressif.com/projects/esp-modbus/en/stable>`__
* `旧版本 v1.x.x 文档 <https://docs.espressif.com/projects/esp-modbus/en/v1>`__

应用示例
~~~~~~~~

自 ESP-IDF v6.0 版本起，已从 ESP-IDF 中移除已废弃的 ``esp-modbus v1`` 组件示例。

- `旧版 esp-modbus v1.x.x 示例 (esp-idf v5.5) <https://github.com/espressif/esp-idf/tree/release/v5.5/examples/protocols/modbus>`__

以下示例分别演示了串行和 TCP 端口的 ESP-Modbus 库在从站和主站实现中的应用。

- `mb_serial_slave <https://github.com/espressif/esp-modbus/tree/main/examples/serial/mb_serial_slave>`__ - 演示如何将 {IDF_TARGET_NAME} 用作 Modbus 串行从站设备，通过 esp-modbus 协议栈，使外部 Modbus 主机能够使用 Modbus 协议读写设备参数。

- `mb_serial_master <https://github.com/espressif/esp-modbus/tree/main/examples/serial/mb_serial_master>`__ - 演示如何在 {IDF_TARGET_NAME} 上使用 esp-modbus 协议栈端口作为 Modbus 串行主站设备，能够读写 Modbus 段中从站设备的值。

- `mb_tcp_slave <https://github.com/espressif/esp-modbus/tree/main/examples/tcp/mb_tcp_slave>`__ - 演示 esp-modbus TCP 从站协议栈端口，允许外部 Modbus 主机通过 Modbus 协议读写设备参数。

- `mb_tcp_master <https://github.com/espressif/esp-modbus/tree/main/examples/tcp/mb_tcp_master>`__ - 演示如何在 {IDF_TARGET_NAME} 上使用 esp-modbus 协议栈端口作为 Modbus TCP 主站设备，能够读写 Modbus 网络中从站设备的值。

有关详细信息，请参阅每个具体示例的 ``README.md`` 文档。

讨论
~~~~

* `v2 版本讨论 <https://github.com/espressif/esp-modbus/discussions>`__
