Wi-Fi 库
========

:link_to_translation:`en:[English]`

概述
-----

Wi-Fi 库支持配置及监控 {IDF_TARGET_NAME} Wi-Fi 连网功能。

支持配置：

- 基站模式（即 STA 模式或 Wi-Fi 客户端模式），此时 {IDF_TARGET_NAME} 连接到接入点 (AP)。
- AP 模式（即 Soft-AP 模式或接入点模式），此时基站连接到 {IDF_TARGET_NAME}。
- AP-STA 共存模式（{IDF_TARGET_NAME} 既是接入点，同时又作为基站连接到另外一个接入点）。

- 上述模式的各种安全模式（WPA、WPA2 及 WEP 等）。
- 扫描接入点（包括主动扫描及被动扫描）。
- 使用混杂模式监控 IEEE802.11 Wi-Fi 数据包。


应用示例
----------

ESP-IDF 示例项目的 :example:`wifi` 目录下包含以下应用程序：

* Wi-Fi 示例代码；

* 另外一个简单的应用程序 `esp-idf-template <https://github.com/espressif/esp-idf-template>`_，演示了如何将 {IDF_TARGET_NAME} 模组连接到 AP。


API 参考
-------------

.. include-build-file:: inc/esp_wifi.inc
.. include-build-file:: inc/esp_wifi_types.inc
