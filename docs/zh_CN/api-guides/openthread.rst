OpenThread
==========

:link_to_translation:`en:[English]`

`OpenThread <https://github.com/openthread/openthread>`_ 是在 802.15.4 MAC 层上运行的 IP 协议栈，支持 mesh 网络，具有低功耗特性。

OpenThread 协议栈运行模式
-------------------------

在乐鑫的芯片上，OpenThread 可按以下模式运行：

独立节点模式
++++++++++++

在此模式下，完整的 OpenThread 协议栈及其应用层在同一芯片上运行，适用于支持 15.4 无线通信协议的芯片，如 ESP32-H2, ESP32-C6。

无线协处理器 (RCP) 模式
+++++++++++++++++++++++++

在此模式下，芯片通过连接到运行 OpenThread IP 协议栈的另一个主机，代表主机发送和接收 15.4 数据包。该模式适用于支持 15.4 无线通信协议的芯片，如 ESP32-H2, ESP32-C6。对于芯片和主机之间的通信方式，目前 ESP-IDF 支持 SPI 或 UART。考虑到传输延迟，建议使用 SPI。

OpenThread 主机模式
+++++++++++++++++++

在此模式下，不支持 15.4 无线通信协议的芯片可以连接到 RCP，并在主机模式下运行 OpenThread。这种模式支持在 Wi-Fi 芯片上（如 ESP32、ESP32-S2、ESP32-S3 和 ESP32-C3 等）运行 OpenThread。下图展示了设备在不同模式下的工作方式：

.. blockdiag::
    :caption: OpenThread 设备模式
    :align: center

    blockdiag openthread-device-modes {

        # global attributes
        node_height = 90;
        node_width = 220;
        span_width = 100
        default_shape = roundedbox;
        default_group_color = none;

        # node labels
        HOST_NODE [label="OpenThread \nhost\n(ESP32)", fontsize=14];
        RCP [label="Radio \nCo-Processor\n(ESP32-H2)", fontsize=14];
        STANDALONE [label="Standalone \nnode\n (ESP32-H2)", fontsize=14];

        # node connections + labels
        RCP -> STANDALONE [label="15.4 radio", dir=both, style=dashed];

        # arrange nodes vertically
        group {
           orientation = portrait;
           HOST_NODE -> RCP [label="SPI", dir=both];
        }
    }


编写 OpenThread 应用程序
--------------------------

要学习编写 OpenThread 应用程序，可以从 OpenThread 应用示例 :example:`openthread/ot_cli` 开始。该示例中展示了简单的 OpenThread 网络组网流程，以及在 Openthread 网络上，如何实现基于套接字的服务器和客户端之间的简单通信。

初始化 OpenThread 协议栈所需的准备
++++++++++++++++++++++++++++++++++++

- s1.1： 主任务调用 :cpp:func:`esp_vfs_eventfd_register`，初始化 eventfd 虚拟文件系统。eventfd 文件系统用于实现 OpenThread 协议栈中的任务通知。

- s1.2： 主任务调用 :cpp:func:`nvs_flash_init` 初始化 NVS，即 Thread 网络数据的存储位置。

- s1.3： **可选**。主任务调用 :cpp:func:`esp_netif_init`，为 Thread 创建网络接口。

- s1.4： 主任务调用 :cpp:func:`esp_event_loop_create` 创建系统事件任务，并初始化应用程序事件的回调函数。

OpenThread 协议栈初始化
++++++++++++++++++++++++++

- s2.1： 调用 :cpp:func:`esp_openthread_init` 初始化 OpenThread 协议栈。

OpenThread 网络接口初始化
++++++++++++++++++++++++++++

以下为 **可选** 步骤，仅在应用程序需为 Thread 创建网络接口时使用。

- s3.1：使用 ``ESP_NETIF_DEFAULT_OPENTHREAD`` 调用 :cpp:func:`esp_netif_new`，创建网络接口。
- s3.2：调用 :cpp:func:`esp_openthread_netif_glue_init`，创建 OpenThread 网络接口处理程序。
- s3.3：调用 :cpp:func:`esp_netif_attach` 将处理程序附加到网络接口。

OpenThread 主循环
++++++++++++++++++++

- s4.3：调用 :cpp:func:`esp_openthread_launch_mainloop` 启动 OpenThread 主循环。注意，OpenThread 主循环属于忙等循环，仅在 OpenThread 协议栈终止后返回。

调用 OpenThread API
++++++++++++++++++++++

OpenThread API 非线程安全。当从其他任务中调用 OpenThread API 时，请确保以 :cpp:func:`esp_openthread_lock_acquire` 获取锁，并在之后以 :cpp:func:`esp_openthread_lock_release` 释放锁。

卸载 Openthread 协议栈
++++++++++++++++++++++++

要在应用程序中卸载 OpenThread 协议栈，请遵循以下步骤：

- 如果创建了 OpenThread 网络接口，请调用 :cpp:func:`esp_netif_destroy` 和 :cpp:func:`esp_openthread_netif_glue_deinit` 卸载 OpenThread 协议栈。
- 调用 :cpp:func:`esp_openthread_deinit` 卸载 OpenThread 协议栈。

OpenThread 边界路由器
---------------------

OpenThread 边界路由器连接了 Thread 网络和其他 IP 网络，提供 IPv6 连通性、服务注册和委托功能。

要在 ESP 芯片上启用 OpenThread 边界路由器，需要将 RCP 连接到具备 Wi-Fi 功能的芯片上，如 ESP32。

在初始化过程中，调用 :cpp:func:`esp_openthread_border_router_init` 会启用所有边界路由功能。

要了解更多有关边界路由器的详细信息，请参阅 :example:`openthread/ot_br` 示例和其中的 README 文件。
