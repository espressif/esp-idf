Wi-Fi 库
========

:link_to_translation:`en:[English]`

概述
-----

Wi-Fi 库支持配置及监控 {IDF_TARGET_NAME} Wi-Fi 连网功能。支持配置：

- station 模式（即 STA 模式或 Wi-Fi 客户端模式），此时 {IDF_TARGET_NAME} 连接到接入点 (AP)。
- AP 模式（即 Soft-AP 模式或接入点模式），此时 station 连接到 {IDF_TARGET_NAME}。
- station/AP 共存模式（{IDF_TARGET_NAME} 既是接入点，同时又作为 station 连接到另外一个接入点）。

- 上述模式的各种安全模式（WPA、WPA2、WPA3 等）。
- 扫描接入点（包括主动扫描及被动扫描）。
- 使用混杂模式监控 IEEE802.11 Wi-Fi 数据包。


应用示例
----------

ESP-IDF 仓库的 :example:`wifi` 目录下提供了演示 Wi-Fi 库功能的几个应用示例，请查看 :example_file:`README <wifi/README.md>` 了解更多详细信息。


API 参考
-------------

.. include-build-file:: inc/esp_wifi.inc
.. include-build-file:: inc/esp_wifi_types.inc
.. include-build-file:: inc/esp_wifi_types_generic.inc
.. include-build-file:: inc/esp_eap_client.inc
.. include-build-file:: inc/esp_wps.inc
.. include-build-file:: inc/esp_rrm.inc
.. include-build-file:: inc/esp_wnm.inc
.. include-build-file:: inc/esp_mbo.inc
