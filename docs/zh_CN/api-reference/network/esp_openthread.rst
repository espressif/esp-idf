Thread
======

:link_to_translation:`en:[English]`

概述
----

`Thread <https://www.threadgroup.org>`_ 是一个基于 IP 的网状网络协议,它基于 802.15.4 物理层和 MAC 层。

应用示例
--------------------

ESP-IDF 示例目录 :example:`openthread` 包含以下应用程序：

- OpenThread 交互 shell：:example:`openthread/ot_cli`
- 边界路由器 (Thread Border Router)：:example:`openthread/ot_br`
- Thread 无线电协处理器 (Thread Radio Co-Processor)：:example:`openthread/ot_rcp`


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
