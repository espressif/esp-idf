连网 API
***************

:link_to_translation:`en:[English]`

.. only:: SOC_WIFI_SUPPORTED

   Wi-Fi
   =====

   .. toctree::
      :maxdepth: 1

      esp_now
      :SOC_WIFI_MESH_SUPPORT: esp-wifi-mesh
      esp_smartconfig
      esp_wifi
      esp_dpp
      :SOC_WIFI_NAN_SUPPORT: esp_nan

   本部分的 Wi-Fi API 示例代码存放在 ESP-IDF 示例项目的 :example:`wifi` 目录下。

.. only:: SOC_WIFI_MESH_SUPPORT

      ESP-WIFI-MESH 的示例代码存放在 ESP-IDF 示例项目的 :example:`mesh` 目录下。



以太网
========

.. toctree::
   :maxdepth: 1

   esp_eth

本部分的以太网 API 示例代码存放在 ESP-IDF 示例项目的 :example:`ethernet` 目录下。

Thread
========

.. toctree::
   :maxdepth: 1

   esp_openthread

Thread 是一种基于 IPv6 的物联网网状网络技术。

本部分的 Thread API 示例代码存放在 ESP-IDF 示例项目的 :example:`openthread` 目录下。

ESP-NETIF
=========

.. toctree::
   :maxdepth: 1

   esp_netif

IP 网络层协议
================

.. toctree::
   :hidden:

   esp_netif_driver

TCP/IP 套接字 API 的示例代码存放在 ESP-IDF 示例项目的 :example:`protocols/sockets` 目录下。

应用层协议 
===========

应用层网络协议（IP 网络层协议之上）的相关文档存放在 :doc:`../protocols/index` 目录下。
