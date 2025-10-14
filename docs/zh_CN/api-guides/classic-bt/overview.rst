概述
=======

:link_to_translation:`en:[English]`

此文档概述了 ESP-IDF 中经典蓝牙协议栈的架构，并提供了一些相关文档和应用示例的快速链接。

.. only:: esp32

    {IDF_TARGET_NAME} 支持双模蓝牙 4.2，并且已经获得双模蓝牙 4.2 认证。

ESP-IDF 中的经典蓝牙协议栈是一个分层架构，可在 {IDF_TARGET_NAME} 系列芯片上实现经典蓝牙功能，详见下图。

.. only:: esp32

    .. figure:: ../../../_static/classic-bluetooth-architecture.png
        :align: center
        :scale: 90%
        :alt: {IDF_TARGET_NAME} 经典蓝牙协议栈架构

        {IDF_TARGET_NAME} 经典蓝牙协议栈架构


以下各节简要介绍了每个层，并提供了相关文档和应用示例的快速链接。


ESP 蓝牙控制器
--------------

底层为 ESP 蓝牙控制器，包含 PHY、基带、链路控制器、链路管理器、设备管理器和 HCI 等各种模块。该层管理硬件接口和链路，以库的形式提供功能，并通过 API 访问，且直接与硬件和低级别蓝牙协议交互。

- :doc:`API 参考 <../../api-reference/bluetooth/controller_vhci>`
- :example:`应用示例 <bluetooth/hci/controller_hci_uart_esp32>`


ESP 蓝牙主机
-------------

IDF 中的 ESP-Bluedroid 主机支持经典蓝牙。


ESP-Bluedroid
^^^^^^^^^^^^^

ESP-Bluedroid 是原生 Android 蓝牙协议栈 Bluedroid 的修改版，由两层组成：蓝牙上层 (BTU) 和蓝牙传输控制器层 (BTC)。BTU 层负责处理 L2CAP 等底层蓝牙协议以及其他配置文件，提供以 "bta" 为前缀的接口。BTC 层主要负责向应用层提供以 "esp" 为前缀的支持接口，并处理其他任务。所有的 API 都位于 ESP_API 层，开发者应使用以 "esp" 为前缀的经典蓝牙 API。

- API 参考

  - :doc:`../../api-reference/bluetooth/bt_common`
  - :doc:`经典蓝牙 <../../api-reference/bluetooth/classic_bt>`
- :example:`应用程序示例 <bluetooth/bluedroid/classic_bt>`


应用
----

最上层是应用层。利用上述 API 和蓝牙规范，可以在 ESP-Bluedroid 协议栈之上创建特定用例的经典蓝牙应用程序。
