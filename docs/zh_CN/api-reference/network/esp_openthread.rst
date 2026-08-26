Thread
======

:link_to_translation:`en:[English]`

概述
----

`Thread <https://www.threadgroup.org>`_ 是一个基于 IP 的网状网络协议,它基于 802.15.4 物理层和 MAC 层。

应用示例
--------------------

- :example:`openthread/ot_br` 演示了如何在 {IDF_TARGET_NAME} 上设置 Thread 边界路由器，启用双向 IPv6 连接、服务发现等功能。

- :example:`openthread/ot_cli` 演示了如何使用支持 TCP、UDP 和 Iperf 等附加功能的 OpenThread 命令行界面。该示例需要在配备 IEEE 802.15.4 模块的开发板上运行，并说明了如何使用至少两个这样的开发板进行网络配置。

- :example:`openthread/ot_rcp` 演示了如何与主处理器配合使用，作为 Thread 边界路由器和 Thread Sniffer。该示例需要在配备 IEEE 802.15.4 模块的开发板上运行。

- :example:`openthread/ot_trel` 演示了 Thread Radio Encapsulation Link (TREL) 功能。该示例需要在配备 Wi-Fi 模块的开发板上运行。

- :example:`openthread/ot_sleepy_device/deep_sleep` 演示了 Thread 深度睡眠功能。

- :example:`openthread/ot_sleepy_device/light_sleep` 演示了 Thread 浅睡眠功能。

API参考
-------------

应使用 OpenThread API 操作 Thread 网络。请参考 `OpenThread API 文档 <https://openthread.io/reference>`_。

ESP-IDF 提供额外的 API，用于启动和管理 OpenThread 实现执行网络接口绑定和边界路由功能。

.. include-build-file:: inc/esp_openthread.inc
.. include-build-file:: inc/esp_openthread_types.inc
.. include-build-file:: inc/esp_openthread_lock.inc
.. include-build-file:: inc/esp_openthread_netif_glue.inc
.. include-build-file:: inc/esp_openthread_border_router.inc
