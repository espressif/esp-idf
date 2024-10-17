Wi-Fi Easy Connect\ :sup:`TM` (DPP)
===================================

:link_to_translation:`en:[English]`

Wi-Fi Easy Connect\ :sup:`TM` 是 Wi-Fi Alliance 认证的配网协议，也称为设备配网协议 (DPP) 或 Easy Connect，是一种安全和标准化的 Wi-Fi 设备配网协议。使用 Easy Connect 将新设备添加入网就像扫描二维码一样简单，特别是对于没有 UI 的智能家居和物联网产品而言，大大降低了联网复杂性，加强了的用户体验。与旧的协议如 Wi-Fi Protected Setup (WPS) 等旧协议相比，Wi-Fi Easy Connect 的公钥加密技术额外确保了添加新设备时的网络安全。

Easy Connect 从以下几个方面改善了用户体验：

  - 操作简单直观，设置新设备时无需阅读冗长的指南
  - 无需记住需配网设备的密码或输入密码
  - 支持电子/打印的二维码以及其他人类可读的字符串
  - 同时支持 WPA2 和 WPA3 网络

如需了解更多信息，请参考 Wi-Fi Alliance 的官方介绍：`Easy Connect <https://www.wi-fi.org/discover-wi-fi/wi-fi-easy-connect>`_。

{IDF_TARGET_NAME} 支持 Easy Connect 的二维码配网模式，用户需要使用显示器显示二维码，随后使用兼容的设备扫描此二维码，并将 {IDF_TARGET_NAME} 添加到自己的 Wi-Fi 网络中。此兼容设备需连接到无需支持 Wi-Fi Easy Connect\ :sup:`TM` 的 AP 上。

Easy Connect 协议仍在不断发展。目前已知支持二维码的平台为部分运行 Android 10 及更高系统版本的 Android 智能手机等。使用 Easy Connect 时，无需在智能手机上安装额外的应用程序。

应用示例
-------------------

- :example:`wifi/wifi_easy_connect/dpp-enrollee` 演示如何使用 DPP 将 {IDF_TARGET_NAME} 配置为注册设备，通过二维码和 Android 10 及以上的设备安全地将 ESP 设备接入网络。

API 参考
-------------

.. include-build-file:: inc/esp_dpp.inc
