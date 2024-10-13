Wi-Fi Aware\ :sup:`TM` (NAN)
===================================

:link_to_translation:`en:[English]`

Wi-Fi Aware\ :sup:`TM`，也可称为 NAN (Neighbor Awareness Networking) 协议，其支持 Wi-Fi 设备发现附近的其他服务。通常情况下，基于位置的服务需通过服务器查询环境信息，并通过 GPS 或其他位置推算技术获取定位。不过，NAN 无需与服务器、GPS 或其他地理位置服务保持实时连接，即可支持设备之间通过 Wi-Fi 直接连接来交换信息。NAN 能够在 Wi-Fi 密集的环境中高效扩展，并通过提供附近人员和服务的信息来完善 Wi-Fi 连接性。

多个邻近的 NAN 设备组成一个 NAN 集群，集群中的设备能够相互通信。NAN 设备还可通过 NAN 服务发现协议，通过发布或订阅功能，在所处集群内提供或查找服务。通过服务名称可以完成服务匹配，一旦找到匹配，设备就可以发送信息，或与匹配到的设备间建立 IPv6 数据路径。

{IDF_TARGET_NAME} 支持独立模式下的 Wi-Fi Aware，同时支持服务发现协议和数据路径。Wi-Fi Aware 协议仍在改进中，如需了解更多信息，请前往 Wi-Fi Alliance 官网的 `Wi-Fi Aware <https://www.wi-fi.org/discover-wi-fi/wi-fi-aware>`_ 页面。大多数 Android 8 及更高版本的 Android 智能手机都支持 Wi-Fi Aware。如需了解更多信息，请参阅 Android 的开发者指南 `Wi-Fi Aware <https://www.wi-fi.org/discover-wi-fi/wi-fi-aware>`_。

应用示例
-------------------

- :example:`wifi/wifi_aware/nan_console` 演示如何使用 NAN 协议发现附近的服务、建立数据路径，并在设备之间进行通信，无需互联网或 AP 连接。它提供了配置 NAN 服务、发布或订阅服务、发送消息以及启动或终止数据路径的控制台命令。

- :example:`wifi/wifi_aware/nan_publisher` 演示如何使用 NAN 协议发布服务，将服务广播给附近的其他设备，以及如何响应订阅该服务的设备。

- :example:`wifi/wifi_aware/nan_subscriber` 演示如何使用 NAN 协议发现附近发布所需服务的其他设备，并通过发送消息或启动数据路径与它们进行通信。

API 参考
-------------

.. include-build-file:: inc/esp_nan.inc
