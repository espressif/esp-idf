SmartConfig
===========

:link_to_translation:`en:[English]`

概述
-----

SmartConfig\ :sup:`TM` 是由 TI 开发的配网技术，用于将新的 Wi-Fi 设备连接到 Wi-Fi 网络。它使用移动应用程序将无线网凭据从智能手机或平板电脑端广播给未配网的 Wi-Fi 设备。

这项技术的优势在于，设备无需直接获知 AP 的 SSID 或密码，而是通过智能手机获取。这对于没有用户界面的无头设备和系统而言十分重要。

目前， {IDF_TARGET_NAME} 支持三种类型的 SmartConfig 配网： Airkiss、ESPTouch 和 ESPTouch v2。ESPTouch v2 自 SmartConfig v3.0 （SmartConfig 的版本可以从 :cpp:func:`esp_smartconfig_get_version()` 获取）起开始支持，ESPTouch v2 和 vESPTouch 采用完全不同的配网算法，因此配网速度更快。此外，ESPTouch v2 还增加了 AES 加密功能和自定义数据字段。

从 SmartConfig v3.0.2 开始，ESPTouch v2 的 AES 加密支持随机 IV。在应用程序端，当随机 IV 的选项关闭的时候，默认的 IV 为 0，与旧版本保持一致，当随机 IV 的选项打开的时候，IV 为随机值。需要注意的是，当启用 AES 加密且 IV 为随机值时，配网时间会延长，因为需要将 IV 传输到配网设备。在配网设备端，设备会根据配网包中的 flag 来识别 AES 的随机 IV 是否开启。

如需通过其他方式为 {IDF_TARGET_NAME} 设备配网，请参阅 :doc:`../provisioning/index`。


应用示例
------------

前往 :example:`wifi/smart_config`，查看使用 SmartConfig 将 {IDF_TARGET_NAME} 连接到目标 AP 的应用示例。


API 参考
----------

.. include-build-file:: inc/esp_smartconfig.inc
