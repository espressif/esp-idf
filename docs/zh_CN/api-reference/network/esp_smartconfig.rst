SmartConfig
===========

:link_to_translation:`en:[English]`

SmartConfig\ :sup:`TM` 是由 TI 开发的配网技术，用于将新的 Wi-Fi 设备连接到 Wi-Fi 网络。它使用移动应用程序将无线网凭据从智能手机或平板电脑端广播给未配网的 Wi-Fi 设备。

这项技术的优势在于，设备无需直接获知 AP 的 SSID 或密码，而是通过智能手机获取。这对于没有用户界面的无头设备和系统而言十分重要。

如需通过其他方式为 {IDF_TARGET_NAME} 设备配网，请参阅 :doc:`../provisioning/index`。


应用示例
------------

前往 :example:`wifi/smart_config`，查看使用 SmartConfig 将 {IDF_TARGET_NAME} 连接到目标 AP 的应用示例。


API 参考
----------

.. include-build-file:: inc/esp_smartconfig.inc
