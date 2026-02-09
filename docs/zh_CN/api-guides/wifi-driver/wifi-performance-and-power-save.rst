Wi-Fi 性能与节能
=====================

:link_to_translation:`en:[English]`

.. _wifi-buffer-usage:

Wi-Fi 缓冲区使用情况
--------------------------

本节只介绍动态缓冲区配置。

缓冲区配置的重要性
+++++++++++++++++++++++++++++++++++++++

为了获得一个具有强健、高性能的系统，我们需要非常谨慎地考虑内存的使用或配置情况，因为：

 - {IDF_TARGET_NAME} 的可用内存有限。
 - 目前，LwIP 和 Wi-Fi 驱动程序中默认的缓冲区类型是“动态”，**意味着 LwIP 和 Wi-Fi 都与应用程序共享内存**。程序员应该时刻牢记这一点，否则将面临如“堆内存耗尽”等的内存问题。
 - “堆耗尽”情况非常危险，会导致 {IDF_TARGET_NAME} 出现“未定义行为”。因此，应该为应用程序预留足够的堆内存，防止耗尽。
 - Wi-Fi 的吞吐量很大程度上取决于与内存相关的配置，如 TCP 窗口大小、Wi-Fi 接收/发送数据动态缓冲区数量等。
 - {IDF_TARGET_NAME} LwIP/Wi-Fi 可能使用的堆内存峰值取决于许多因素，例如应用程序可能拥有的最大 TCP/UDP 连接等。
 - 在考虑内存配置时，应用程序所需的总内存也是一个重要因素。

由于这些原因，不存在一个适合所有应用程序的配置。相反，我们必须为每个不同的应用程序考虑不同的内存配置。

动态与静态缓冲区
++++++++++++++++++++++++++++++

Wi-Fi 驱动程序中默认的缓存类型是“动态”。大多数情况下，动态缓冲区可以极大地节省内存。但是因为应用程序需要考虑 Wi-Fi 的内存使用情况，会给应用程序编程造成一定的难度。

lwIP 还在 TCP/IP 层分配缓冲区，这种缓冲区分配也是动态的。具体内容，见 `lwIP 文档内存使用和性能部分 <https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/api-guides/lwip.html#minimum-ram-usage>`_。

Wi-Fi 动态缓冲区峰值
++++++++++++++++++++++++++++++

Wi-Fi 驱动程序支持多种类型的缓冲区（参考 :ref:`Wi-Fi 缓冲区配置 <wifi-buffer-configure>` ）。但本节只介绍 Wi-Fi 动态缓冲的使用方法
Wi-Fi 使用的堆内存峰值是 Wi-Fi 驱动程序 **理论上消耗的最大内存**。通常，该内存峰值取决于：

- :math:`b_{rx}` 配置的动态接收数据缓冲区数
- :math:`b_{tx}` 配置的动态发送数据缓冲区数
- :math:`m_{rx}` Wi-Fi 驱动程序可以接收的最大数据包
- :math:`m_{tx}` Wi-Fi 驱动程序可以发送的最大数据包

因此，Wi-Fi 驱动程序消耗的内存峰值（:math:`p`）可以用下面的公式计算：

.. math::

    p = (b_{rx} * m_{rx}) + (b_{tx} * m_{tx})

一般情况下，不需要关心动态发送数据长缓冲区和超长缓冲区，因为它们是管理帧，对系统的影响很小。


{IDF_TARGET_NAME} Wi-Fi 吞吐量
-----------------------------------

下表是我们在 Espressif 实验室和屏蔽箱中获得的最佳吞吐量结果。

.. only:: esp32

    .. list-table::
       :header-rows: 1
       :widths: 10 10 10 15 20

       * - 类型/吞吐量
         - 实验室空气状况
         - 屏蔽箱
         - 测试工具
         - IDF 版本 (commit ID)
       * - 原始 802.11 数据包接收数据
         - N/A
         - **130 MBit/s**
         - 内部工具
         - N/A
       * - 原始 802.11 数据包发送数据
         - N/A
         - **130 MBit/s**
         - 内部工具
         - N/A
       * - UDP 接收数据
         - 30 MBit/s
         - 85 MBit/s
         - iperf example
         - 15575346
       * - UDP 发送数据
         - 30 MBit/s
         - 75 MBit/s
         - iperf example
         - 15575346
       * - TCP 接收数据
         - 20 MBit/s
         - 65 MBit/s
         - iperf example
         - 15575346
       * - TCP 发送数据
         - 20 MBit/s
         - 75 MBit/s
         - iperf example
         - 15575346

    使用 iperf example 测试吞吐量时，sdkconfig 是 :idf_file:`examples/wifi/iperf/sdkconfig.defaults.esp32`。

.. only:: esp32s2

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15 20

        * - 类型/吞吐量
          - 实验室空气状况
          - 屏蔽箱
          - 测试工具
          - IDF 版本 (commit ID)
        * - 原始 802.11 数据包接收数据
          - N/A
          - **130 MBit/s**
          - 内部工具
          - N/A
        * - 原始 802.11 数据包发送数据
          - N/A
          - **130 MBit/s**
          - 内部工具
          - N/A
        * - UDP 接收数据
          - 30 MBit/s
          - 70 MBit/s
          - iperf example
          - 15575346
        * - UDP 发送数据
          - 30 MBit/s
          - 50 MBit/s
          - iperf example
          - 15575346
        * - TCP 接收数据
          - 20 MBit/s
          - 32 MBit/s
          - iperf example
          - 15575346
        * - TCP 发送数据
          - 20 MBit/s
          - 37 MBit/s
          - iperf example
          - 15575346

    使用 iperf example 测试吞吐量时，sdkconfig 是 :idf_file:`examples/wifi/iperf/sdkconfig.defaults.esp32s2`。

.. only:: esp32c3

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15 20

        * - 类型/吞吐量
          - 实验室空气状况
          - 屏蔽箱
          - 测试工具
          - IDF 版本 (commit ID)
        * - 原始 802.11 数据包接收数据
          - N/A
          - **130 MBit/s**
          - 内部工具
          - N/A
        * - 原始 802.11 数据包发送数据
          - N/A
          - **130 MBit/s**
          - 内部工具
          - N/A
        * - UDP 接收数据
          - 30 MBit/s
          - 50 MBit/s
          - iperf example
          - 15575346
        * - UDP 发送数据
          - 30 MBit/s
          - 40 MBit/s
          - iperf example
          - 15575346
        * - TCP 接收数据
          - 20 MBit/s
          - 35 MBit/s
          - iperf example
          - 15575346
        * - TCP 发送数据
          - 20 MBit/s
          - 37 MBit/s
          - iperf example
          - 15575346

    使用 iperf example 测试吞吐量时，sdkconfig 是 :idf_file:`examples/wifi/iperf/sdkconfig.defaults.esp32c3`。

.. only:: esp32c5

    - 2.4 GHz 频段

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15 20

        * - 类型/吞吐量
          - 实验室空气状况
          - 屏蔽箱
          - 测试工具
          - IDF 版本 (commit ID)
        * - 原始 802.11 数据包接收数据
          - N/A
          - **130 MBit/s**
          - 内部工具
          - N/A
        * - 原始 802.11 数据包发送数据
          - N/A
          - **130 MBit/s**
          - 内部工具
          - N/A
        * - UDP 接收数据
          - 30 MBit/s
          - 68 MBit/s
          - iperf example
          - 7ff0a07d
        * - UDP 发送数据
          - 30 MBit/s
          - 63 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP 接收数据
          - 20 MBit/s
          - 59 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP 发送数据
          - 20 MBit/s
          - 49 MBit/s
          - iperf example
          - 7ff0a07d

    - 5 GHz 频段

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15 20

        * - 类型/吞吐量
          - 实验室空气状况
          - 屏蔽箱
          - 测试工具
          - IDF 版本 (commit ID)
        * - 原始 802.11 数据包接收数据
          - N/A
          - **130 MBit/s**
          - 内部工具
          - N/A
        * - 原始 802.11 数据包发送数据
          - N/A
          - **130 MBit/s**
          - 内部工具
          - N/A
        * - UDP 接收数据
          - 30 MBit/s
          - 71 MBit/s
          - iperf example
          - 7ff0a07d
        * - UDP 发送数据
          - 30 MBit/s
          - 64 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP 接收数据
          - 20 MBit/s
          - 61 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP 发送数据
          - 20 MBit/s
          - 50 MBit/s
          - iperf example
          - 7ff0a07d

    使用 iperf example 测试吞吐量时，sdkconfig 是 :idf_file:`examples/wifi/iperf/sdkconfig.defaults.esp32c5`。

.. only:: esp32c6

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15 20

        * - 类型/吞吐量
          - 实验室空气状况
          - 屏蔽箱
          - 测试工具
          - IDF 版本 (commit ID)
        * - 原始 802.11 数据包接收数据
          - N/A
          - **130 MBit/s**
          - 内部工具
          - N/A
        * - 原始 802.11 数据包发送数据
          - N/A
          - **130 MBit/s**
          - 内部工具
          - N/A
        * - UDP 接收数据
          - 30 MBit/s
          - 63 MBit/s
          - iperf example
          - 7ff0a07d
        * - UDP 发送数据
          - 30 MBit/s
          - 51 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP 接收数据
          - 20 MBit/s
          - 46 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP 发送数据
          - 20 MBit/s
          - 43 MBit/s
          - iperf example
          - 7ff0a07d

    使用 iperf example 测试吞吐量时，sdkconfig 是 :idf_file:`examples/wifi/iperf/sdkconfig.defaults.esp32c6`。

.. only:: esp32c61

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15 20

        * - 类型/吞吐量
          - 实验室空气状况
          - 屏蔽箱
          - 测试工具
          - IDF 版本 (commit ID)
        * - 原始 802.11 数据包接收数据
          - N/A
          - **130 MBit/s**
          - 内部工具
          - N/A
        * - 原始 802.11 数据包发送数据
          - N/A
          - **130 MBit/s**
          - 内部工具
          - N/A
        * - UDP 接收数据
          - 30 MBit/s
          - 68 MBit/s
          - iperf example
          - 7ff0a07d
        * - UDP 发送数据
          - 30 MBit/s
          - 53 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP 接收数据
          - 20 MBit/s
          - 45 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP 发送数据
          - 20 MBit/s
          - 37 MBit/s
          - iperf example
          - 7ff0a07d

    使用 iperf example 测试吞吐量时，sdkconfig 是 :idf_file:`examples/wifi/iperf/sdkconfig.defaults.esp32c61`。

.. only:: esp32s3

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15 20

        * - 类型/吞吐量
          - 实验室空气状况
          - 屏蔽箱
          - 测试工具
          - IDF 版本 (commit ID)
        * - 原始 802.11 数据包接收数据
          - N/A
          - **130 MBit/s**
          - 内部工具
          - N/A
        * - 原始 802.11 数据包发送数据
          - N/A
          - **130 MBit/s**
          - 内部工具
          - N/A
        * - UDP 接收数据
          - 30 MBit/s
          - 88 MBit/s
          - iperf example
          - 15575346
        * - UDP 发送数据
          - 30 MBit/s
          - 98 MBit/s
          - iperf example
          - 15575346
        * - TCP 接收数据
          - 20 MBit/s
          - 73 MBit/s
          - iperf example
          - 15575346
        * - TCP 发送数据
          - 20 MBit/s
          - 83 MBit/s
          - iperf example
          - 15575346

    使用 iperf example 测试吞吐量时，sdkconfig 是 :idf_file:`examples/wifi/iperf/sdkconfig.defaults.esp32s3`。


.. _How-to-improve-Wi-Fi-performance:

如何提高 Wi-Fi 性能
----------------------------------

{IDF_TARGET_NAME} Wi-Fi 的性能受许多参数的影响，各参数之间存在相互制约。如果配置得合理，不仅可以提高性能，还可以增加应用程序的可用内存，提高稳定性。

在本节中，我们将简单介绍 Wi-Fi/LWIP 协议栈的工作模式，并说明各个参数的作用。我们将推荐几种配置等级，你可以根据使用场景选择合适的等级。

协议栈工作模式
++++++++++++++++++++++++++++++++++

.. figure:: ../../../_static/api-guides-WiFi-driver-how-to-improve-WiFi-performance.png
    :align: center

    {IDF_TARGET_NAME} 数据路径

{IDF_TARGET_NAME} 协议栈分为四层，分别为应用层、LWIP 层、Wi-Fi 层和硬件层。

 - 在接收过程中，硬件将接收到的数据包放入 DMA 缓冲区，然后依次传送到 Wi-Fi 的接收数据缓冲区、LWIP 的接收数据缓冲区进行相关协议处理，最后传送到应用层。Wi-Fi 的接收数据缓冲区和 LWIP 的接收数据缓冲区默认共享同一个缓冲区。也就是说，Wi-Fi 默认将数据包转发到 LWIP 作为参考。

 - 在发送过程中，应用程序首先将要发送的消息复制到 LWIP 层的发送数据缓冲区，进行 TCP/IP 封装。然后将消息发送到 Wi-Fi 层的发送数据缓冲区进行 MAC 封装，最后等待发送。

参数
++++++++++++++

适当增加上述缓冲区的大小或数量，可以提高 Wi-Fi 性能，但同时，会减少应用程序的可用内存。下面我们将介绍你需要配置的参数：

**接收数据方向：**

 - :ref:`CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM`
    该参数表示硬件层的 DMA 缓冲区数量。提高该参数将增加发送方的一次性接收吞吐量，从而提高 Wi-Fi 协议栈处理突发流量的能力。

 - :ref:`CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM`
    该参数表示 Wi-Fi 层中接收数据缓冲区的数量。提高该参数可以增强数据包的接收性能。该参数需要与 LWIP 层的接收数据缓冲区大小相匹配。

 - :ref:`CONFIG_ESP_WIFI_RX_BA_WIN`
    该参数表示接收端 AMPDU BA 窗口的大小，应配置为 :ref:`CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM` 和 :ref:`CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM` 的二倍数值中较小的数值。

 - :ref:`CONFIG_LWIP_TCP_WND_DEFAULT`
    该参数表示 LWIP 层用于每个 TCP 流的接收数据缓冲区大小，应配置为 WIFI_DYNAMIC_RX_BUFFER_NUM (KB) 的值，从而实现高稳定性能。同时，在有多个流的情况下，应相应降低该参数值。

**发送数据方向：**

 - :ref:`CONFIG_ESP_WIFI_TX_BUFFER`
    该参数表示发送数据缓冲区的类型，建议配置为动态缓冲区，该配置可以充分利用内存。

 - :ref:`CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM`
    该参数表示 Wi-Fi 层发送数据缓冲区数量。提高该参数可以增强数据包发送的性能。该参数值需要与 LWIP 层的发送数据缓冲区大小相匹配。

 - :ref:`CONFIG_LWIP_TCP_SND_BUF_DEFAULT`
    该参数表示 LWIP 层用于每个 TCP 流的发送数据缓冲区大小，应配置为 WIFI_DYNAMIC_TX_BUFFER_NUM (KB) 的值，从而实现高稳定性能。在有多个流的情况下，应相应降低该参数值。

**通过在 IRAM 中放置代码优化吞吐量：**

.. only:: esp32 or esp32s2

    - :ref:`CONFIG_ESP_WIFI_IRAM_OPT`
        如果使能该选项，一些 Wi-Fi 功能将被移至 IRAM，从而提高吞吐量，IRAM 使用量将增加 15 kB。

    - :ref:`CONFIG_ESP_WIFI_RX_IRAM_OPT`
        如果使能该选项，一些 Wi-Fi 接收数据功能将被移至 IRAM，从而提高吞吐量，IRAM 使用量将增加 16 kB。

 - :ref:`CONFIG_LWIP_IRAM_OPTIMIZATION`
    如果使能该选项，一些 LWIP 功能将被移至 IRAM，从而提高吞吐量，IRAM 使用量将增加 13 kB。

.. only:: esp32c6

    - :ref:`CONFIG_ESP_WIFI_IRAM_OPT`
        如果使能该选项，一些 Wi-Fi 功能将被移至 IRAM，从而提高吞吐量，IRAM 使用量将增加 13 kB。

    - :ref:`CONFIG_ESP_WIFI_RX_IRAM_OPT`
        如果使能该选项，一些 Wi-Fi 接收数据功能将被移至 IRAM，从而提高吞吐量，IRAM 使用量将增加 7 kB。

    - :ref:`CONFIG_LWIP_IRAM_OPTIMIZATION`
        如果使能该选项，一些 LWIP 功能将被移至 IRAM，从而提高吞吐量，IRAM 使用量将增加 14 kB。

.. only:: esp32s2

    **缓存：**

     - :ref:`CONFIG_ESP32S2_INSTRUCTION_CACHE_SIZE`
        配置指令缓存的大小。

     - :ref:`CONFIG_ESP32S2_INSTRUCTION_CACHE_LINE_SIZE`
        配置指令缓存总线的宽度。

.. only:: esp32s3

    **缓存：**

     - :ref:`CONFIG_ESP32S3_INSTRUCTION_CACHE_SIZE`
        配置指令缓存的大小。

     - :ref:`CONFIG_ESP32S3_INSTRUCTION_CACHE_LINE_SIZE`
        配置指令缓存总线的宽度。

     - :ref:`CONFIG_ESP32S3_ICACHE_ASSOCIATED_WAYS`
        配置指令缓存相连方式.

     - :ref:`CONFIG_ESP32S3_DATA_CACHE_SIZE`
        配置数据缓存的大小.

     - :ref:`CONFIG_ESP32S3_DATA_CACHE_LINE_SIZE`
        配置数据缓存行大小.

     - :ref:`CONFIG_ESP32S3_DCACHE_ASSOCIATED_WAYS`
        配置数据缓存相连方式.

.. note::
    上述的缓冲区大小固定为 1.6 KB。

如何配置参数
++++++++++++++++++++++++++++

{IDF_TARGET_NAME} 的内存由协议栈和应用程序共享。

在这里，我们给出了几种配置等级。在大多数情况下，应根据应用程序所占用内存的大小，选择合适的等级进行参数配置。

下表中未提及的参数应设置为默认值。

.. only:: esp32

     .. list-table::
        :header-rows: 1
        :widths: 10 5 5 10 5 5 10 5

        * - 等级
          - Iperf
          - 发送数据优先
          - 高性能
          - 接收数据优先
          - 默认值
          - 节省内存
          - 最小
        * - 可用内存 (KB)
          - 37.1
          - 113.8
          - 123.3
          - 145.5
          - 144.5
          - 170.2
          - 185.2
        * - WIFI_STATIC_RX_BUFFER_NUM
          - 16
          - 6
          - 6
          - 6
          - 6
          - 6
          - 4
        * - WIFI_DYNAMIC_RX_BUFFER_NUM
          - 64
          - 16
          - 24
          - 34
          - 20
          - 12
          - 8
        * - WIFI_DYNAMIC_TX_BUFFER_NUM
          - 64
          - 28
          - 24
          - 18
          - 20
          - 12
          - 8
        * - WIFI_RX_BA_WIN
          - 32
          - 8
          - 12
          - 12
          - 10
          - 6
          - 禁用
        * - TCP_SND_BUF_DEFAULT (KB)
          - 65
          - 28
          - 24
          - 18
          - 20
          - 12
          - 8
        * - TCP_WND_DEFAULT (KB)
          - 65
          - 16
          - 24
          - 34
          - 20
          - 12
          - 8
        * - WIFI_IRAM_OPT
          - 开启
          - 开启
          - 开启
          - 开启
          - 开启
          - 开启
          - 开启
        * - WIFI_RX_IRAM_OPT
          - 开启
          - 开启
          - 开启
          - 开启
          - 开启
          - 开启
          - 开启
        * - LWIP_IRAM_OPTIMIZATION
          - 开启
          - 开启
          - 开启
          - 开启
          - 开启
          - 开启
          - 开启
        * - TCP 发送数据吞吐量 (Mbit/s)
          - 74.6
          - 50.8
          - 46.5
          - 39.9
          - 44.2
          - 33.8
          - 25.6
        * - TCP 接收数据吞吐量 (Mbit/s)
          - 63.6
          - 35.5
          - 42.3
          - 48.5
          - 40.5
          - 30.1
          - 27.8
        * - UDP 发送数据吞吐量 (Mbit/s)
          - 76.2
          - 75.1
          - 74.1
          - 72.4
          - 69.6
          - 64.1
          - 36.5
        * - UDP 接收数据吞吐量 (Mbit/s)
          - 83.1
          - 66.3
          - 75.1
          - 75.6
          - 73.1
          - 65.3
          - 54.7


.. only:: esp32s2

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 10 10 10

        * - 等级
          - Iperf
          - 高性能
          - 默认
          - 节省内存
          - 最小
        * - 可用内存 (KB)
          - 4.1
          - 24.2
          - 78.4
          - 86.5
          - 116.4
        * - WIFI_STATIC_RX_BUFFER_NUM
          - 8
          - 6
          - 6
          - 4
          - 3
        * - WIFI_DYNAMIC_RX_BUFFER_NUM
          - 24
          - 18
          - 12
          - 8
          - 6
        * - WIFI_DYNAMIC_TX_BUFFER_NUM
          - 24
          - 18
          - 12
          - 8
          - 6
        * - WIFI_RX_BA_WIN
          - 12
          - 9
          - 6
          - 4
          - 3
        * - TCP_SND_BUF_DEFAULT (KB)
          - 24
          - 18
          - 12
          - 8
          - 6
        * - TCP_WND_DEFAULT (KB)
          - 24
          - 18
          - 12
          - 8
          - 6
        * - WIFI_IRAM_OPT
          - 开启
          - 开启
          - 开启
          - 开启
          - 关闭
        * - WIFI_RX_IRAM_OPT
          - 开启
          - 开启
          - 开启
          - 关闭
          - 关闭
        * - LWIP_IRAM_OPTIMIZATION
          - 开启
          - 开启
          - 关闭
          - 关闭
          - 关闭
        * - INSTRUCTION_CACHE
          - 16
          - 16
          - 16
          - 16
          - 8
        * - INSTRUCTION_CACHE_LINE
          - 16
          - 16
          - 16
          - 16
          - 16
        * - TCP 发送数据吞吐量 (Mbit/s)
          - 37.6
          - 33.1
          - 22.5
          - 12.2
          - 5.5
        * - TCP 接收数据吞吐量 (Mbit/s)
          - 31.5
          - 28.1
          - 20.1
          - 13.1
          - 7.2
        * - UDP 发送数据吞吐量 (Mbit/s)
          - 58.1
          - 57.3
          - 28.1
          - 22.6
          - 8.7
        * - UDP 接收数据吞吐量 (Mbit/s)
          - 78.1
          - 66.7
          - 65.3
          - 53.8
          - 28.5

.. only:: esp32c3

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15

        * - 等级
          - Iperf
          - 默认
          - 最小
        * - 可用内存 (KB)
          - 59
          - 160
          - 180
        * - WIFI_STATIC_RX_BUFFER_NUM
          - 20
          - 8
          - 3
        * - WIFI_DYNAMIC_RX_BUFFER_NUM
          - 40
          - 16
          - 6
        * - WIFI_DYNAMIC_TX_BUFFER_NUM
          - 40
          - 16
          - 6
        * - WIFI_RX_BA_WIN
          - 32
          - 16
          - 6
        * - TCP_SND_BUF_DEFAULT (KB)
          - 40
          - 16
          - 6
        * - TCP_WND_DEFAULT (KB)
          - 40
          - 16
          - 6
        * - LWIP_IRAM_OPTIMIZATION
          - 开启
          - 开启
          - 关闭
        * - TCP 发送数据吞吐量 (Mbit/s)
          - 38.1
          - 27.2
          - 20.4
        * - TCP 接收数据吞吐量 (Mbit/s)
          - 35.3
          - 24.2
          - 17.4
        * - UDP 发送数据吞吐量 (Mbit/s)
          - 40.6
          - 38.9
          - 34.1
        * - UDP 接收数据吞吐量 (Mbit/s)
          - 52.4
          - 44.5
          - 44.2

.. only:: esp32c6

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15

        * - 等级
          - Iperf
          - 默认
          - 最小
        * - 可用内存 (KB)
          - 223
          - 276
          - 299
        * - WIFI_STATIC_RX_BUFFER_NUM
          - 20
          - 8
          - 3
        * - WIFI_DYNAMIC_RX_BUFFER_NUM
          - 40
          - 16
          - 6
        * - WIFI_DYNAMIC_TX_BUFFER_NUM
          - 40
          - 16
          - 6
        * - WIFI_RX_BA_WIN
          - 32
          - 16
          - 6
        * - TCP_SND_BUF_DEFAULT (KB)
          - 40
          - 16
          - 6
        * - TCP_WND_DEFAULT (KB)
          - 40
          - 16
          - 6
        * - LWIP_IRAM_OPTIMIZATION
          - 开启
          - 开启
          - 关闭
        * - TCP 发送数据吞吐量 (Mbit/s)
          - 30.5
          - 25.9
          - 16.4
        * - TCP 接收数据吞吐量 (Mbit/s)
          - 27.8
          - 21.6
          - 14.3
        * - UDP 发送数据吞吐量 (Mbit/s)
          - 37.8
          - 36.1
          - 34.6
        * - UDP 接收数据吞吐量 (Mbit/s)
          - 41.5
          - 36.8
          - 36.7

.. only:: esp32c2

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15

        * - 等级
          - Iperf
          - 默认
          - 最小
        * - 可用内存 (KB)
          - 37
          - 56
          - 84
        * - WIFI_STATIC_RX_BUFFER_NUM
          - 14
          - 7
          - 3
        * - WIFI_DYNAMIC_RX_BUFFER_NUM
          - 18
          - 14
          - 6
        * - WIFI_DYNAMIC_TX_BUFFER_NUM
          - 18
          - 14
          - 6
        * - WIFI_RX_BA_WIN
          - 16
          - 12
          - 6
        * - TCP_SND_BUF_DEFAULT (KB)
          - 18
          - 14
          - 6
        * - TCP_WND_DEFAULT (KB)
          - 18
          - 14
          - 6
        * - LWIP_IRAM_OPTIMIZATION
          - 开启
          - 开启
          - 关闭
        * - TCP 发送数据吞吐量 (Mbit/s)
          - 21.6
          - 21.4
          - 14.3
        * - TCP 接收数据吞吐量 (Mbit/s)
          - 19.1
          - 17.9
          - 12.4
        * - UDP 发送数据吞吐量 (Mbit/s)
          - 26.4
          - 26.3
          - 25.0
        * - UDP 接收数据吞吐量 (Mbit/s)
          - 32.3
          - 31.5
          - 27.7

.. only:: esp32s3

     .. list-table::
        :header-rows: 1
        :widths: 25 20 25 25

        * - 等级
          - Iperf
          - 默认
          - 最小
        * - 可用内存 (KB)
          - 133.9
          - 183.9
          - 273.6
        * - WIFI_STATIC_RX_BUFFER_NUM
          - 24
          - 8
          - 3
        * - WIFI_DYNAMIC_RX_BUFFER_NUM
          - 64
          - 32
          - 6
        * - WIFI_DYNAMIC_TX_BUFFER_NUM
          - 64
          - 32
          - 6
        * - WIFI_RX_BA_WIN
          - 32
          - 16
          - 6
        * - TCP_SND_BUF_DEFAULT (KB)
          - 64
          - 32
          - 6
        * - TCP_WND_DEFAULT (KB)
          - 64
          - 32
          - 6
        * - WIFI_IRAM_OPT
          - 开启
          - 开启
          - 开启
        * - WIFI_RX_IRAM_OPT
          - 开启
          - 开启
          - 开启
        * - LWIP_IRAM_OPTIMIZATION
          - 开启
          - 开启
          - 关闭
        * - INSTRUCTION_CACHE
          - 32
          - 32
          - 16
        * - INSTRUCTION_CACHE_LINE
          - 32
          - 32
          - 32
        * - INSTRUCTION_CACHE_WAYS
          - 8
          - 8
          - 4
        * - TCP 发送数据吞吐量 (Mbit/s)
          - 83.93
          - 64.28
          - 23.17
        * - TCP 接收数据吞吐量 (Mbit/s)
          - 73.98
          - 60.39
          - 18.11
        * - UDP 发送数据吞吐量 (Mbit/s)
          - 98.69
          - 96.28
          - 48.78
        * - UDP 接收数据吞吐量 (Mbit/s)
          - 88.58
          - 86.57
          - 59.45

.. only:: esp32 or esp32s3

    .. note::
        以上结果使用华硕 RT-N66U 路由器，在屏蔽箱中进行单流测试得出。
        {IDF_TARGET_NAME} 的 CPU 为双核，频率为 240 MHz，flash 为 QIO 模式，频率为 80 MHz。

.. only:: esp32s2

    .. note::
        以上结果使用华硕 RT-N66U 路由器，在屏蔽箱中进行单流测试得出。
        {IDF_TARGET_NAME} 的 CPU 为单核，频率为 240 MHz，flash 为 QIO 模式，频率为 80 MHz。

.. only:: esp32c3

    .. note::
        以上结果使用华硕 RT-N66U 路由器，在屏蔽箱中进行单流测试得出。
        {IDF_TARGET_NAME} 的 CPU 为单核，频率为 160 MHz，flash 为 QIO 模式，频率为 80 MHz。

.. only:: esp32c6

    .. note::
        以上结果使用小米 AX6000 路由器，在屏蔽箱中进行单流测试得出。
        {IDF_TARGET_NAME} 的 CPU 为单核，频率为 160 MHz，flash 为 QIO 模式，频率为 80 MHz。

.. only:: esp32c2

    .. note::
        以上结果使用红米 RM2100 路由器，在屏蔽箱中进行单流测试得出。
        {IDF_TARGET_NAME} 的 CPU 为单核，频率为 120 MHz，flash 为 QIO 模式，频率为 60 MHz。

.. only:: esp32

    **等级：**

     - **Iperf 等级**
        {IDF_TARGET_NAME} 的极端性能等级，用于测试极端性能。

     - **高性能等级**
        {IDF_TARGET_NAME} 的高性能配置等级，适用于应用程序占用内存较少且有高性能要求的场景。在该等级中，可以根据使用场景选择使用接收数据优先等级或发送数据优先等级。

     - **默认等级**
        {IDF_TARGET_NAME} 的默认配置等级、兼顾可用内存和性能。

     - **节省内存等级**
        该等级适用于应用程序需要大量内存的场景，在这一等级下，收发器的性能会有所降低。

     - **最小等级**
        {IDF_TARGET_NAME} 的最小配置等级。协议栈只使用运行所需的内存。适用于对性能没有要求，而应用程序需要大量内存的场景。

.. only:: esp32s2

    **等级：**

     - **Iperf 等级**
        {IDF_TARGET_NAME} 极端性能等级，用于测试极端性能。

     - **高性能等级**
        {IDF_TARGET_NAME} 的高性能配置等级，适用于应用程序占用内存较少且有高性能要求的场景。

     - **默认等级**
        {IDF_TARGET_NAME} 的默认配置等级、兼顾可用内存和性能。

     - **节省内存等级**
        该等级适用于应用程序需要大量内存的场景，在这一等级下，收发器的性能会有所降低。

     - **最小等级**
        {IDF_TARGET_NAME} 的最小配置等级。协议栈只使用运行所需的内存。适用于对性能没有要求，而应用程序需要大量内存的场景。

.. only:: esp32c3 or esp32s3 or esp32c6

    **等级：**

     - **Iperf 等级**
        {IDF_TARGET_NAME} 极端性能等级，用于测试极端性能。

     - **默认等级**
        {IDF_TARGET_NAME} 的默认配置等级、兼顾可用内存和性能。

     - **最小等级**
        {IDF_TARGET_NAME} 的最小配置等级。协议栈只使用运行所需的内存。适用于对性能没有要求，而应用程序需要大量内存的场景。

.. only:: SOC_SPIRAM_SUPPORTED

    使用 PSRAM
    ++++++++++++++++++++++++++++

    PSRAM 一般在应用程序占用大量内存时使用。在该模式下，:ref:`CONFIG_ESP_WIFI_TX_BUFFER` 被强制为静态。:ref:`CONFIG_ESP_WIFI_STATIC_TX_BUFFER_NUM` 表示硬件层 DMA 缓冲区数量，提高这一参数可以增强性能。
    以下是使用 PSRAM 时的推荐等级。

    .. only:: esp32

           .. list-table::
             :header-rows: 1
             :widths: 10 10 10 10 15

             * - 等级
               - Iperf
               - 默认
               - 节省内存
               - 最小
             * - 可用内存 (KB)
               - 113.8
               - 152.4
               - 181.2
               - 202.6
             * - WIFI_STATIC_RX_BUFFER_NUM
               - 16
               - 8
               - 4
               - 2
             * - WIFI_DYNAMIC_RX_BUFFER_NUM
               - 128
               - 128
               - 128
               - 128
             * - WIFI_STATIC_TX_BUFFER_NUM
               - 16
               - 8
               - 4
               - 2
             * - WIFI_RX_BA_WIN
               - 16
               - 16
               - 8
               - 禁用
             * - TCP_SND_BUF_DEFAULT (KB)
               - 65
               - 65
               - 65
               - 65
             * - TCP_WND_DEFAULT (KB)
               - 65
               - 65
               - 65
               - 65
             * - WIFI_IRAM_OPT
               - 开启
               - 开启
               - 开启
               - 关闭
             * - WIFI_RX_IRAM_OPT
               - 开启
               - 开启
               - 关闭
               - 关闭
             * - LWIP_IRAM_OPTIMIZATION
               - 开启
               - 关闭
               - 关闭
               - 关闭
             * - TCP 发送数据吞吐量 (Mbit/s)
               - 37.5
               - 31.7
               - 21.7
               - 14.6
             * - TCP 接收数据吞吐量 (Mbit/s)
               - 31.5
               - 29.8
               - 26.5
               - 21.1
             * - UDP 发送数据吞吐量 (Mbit/s)
               - 69.1
               - 31.5
               - 27.1
               - 24.1
             * - UDP 接收数据吞吐量 (Mbit/s)
               - 40.1
               - 38.5
               - 37.5
               - 36.9

    .. only:: esp32s2

        .. list-table::
             :header-rows: 1
             :widths: 10 10 10 10 15

             * - 等级
               - Iperf
               - 默认
               - 节省内存
               - 最小
             * - 可用内存 (KB)
               - 70.6
               - 96.4
               - 118.8
               - 148.2
             * - WIFI_STATIC_RX_BUFFER_NUM
               - 8
               - 8
               - 6
               - 4
             * - WIFI_DYNAMIC_RX_BUFFER_NUM
               - 64
               - 64
               - 64
               - 64
             * - WIFI_STATIC_TX_BUFFER_NUM
               - 16
               - 8
               - 6
               - 4
             * - WIFI_RX_BA_WIN
               - 16
               - 6
               - 6
               - 禁用
             * - TCP_SND_BUF_DEFAULT (KB)
               - 32
               - 32
               - 32
               - 32
             * - TCP_WND_DEFAULT (KB)
               - 32
               - 32
               - 32
               - 32
             * - WIFI_IRAM_OPT
               - 开启
               - 开启
               - 开启
               - 关闭
             * - WIFI_RX_IRAM_OPT
               - 开启
               - 开启
               - 关闭
               - 关闭
             * - LWIP_IRAM_OPTIMIZATION
               - 开启
               - 关闭
               - 关闭
               - 关闭
             * - INSTRUCTION_CACHE
               - 16
               - 16
               - 16
               - 8
             * - INSTRUCTION_CACHE_LINE
               - 16
               - 16
               - 16
               - 16
             * - DATA_CACHE
               - 8
               - 8
               - 8
               - 8
             * - DATA_CACHE_LINE
               - 32
               - 32
               - 32
               - 32
             * - TCP 发送数据吞吐量 (Mbit/s)
               - 40.1
               - 29.2
               - 20.1
               - 8.9
             * - TCP 接收数据吞吐量 (Mbit/s)
               - 21.9
               - 16.8
               - 14.8
               - 9.6
             * - UDP 发送数据吞吐量 (Mbit/s)
               - 50.1
               - 25.7
               - 22.4
               - 10.2
             * - UDP 接收数据吞吐量 (Mbit/s)
               - 45.3
               - 43.1
               - 28.5
               - 15.1

        .. note::
            达到性能的峰值可能会触发任务看门狗，由于 CPU 可能没有时间处理低优先级的任务，这是一个正常现象。

    .. only:: esp32s3

        **四线 PSRAM：**

          .. list-table::
             :header-rows: 1
             :widths: 25 20 25 25 25

             * - 等级
               - Iperf
               - 默认
               - 节省内存
               - 最小
             * - 可用内存 (KB)
               - 50.3
               - 158.7
               - 198.2
               - 228.9
             * - WIFI_STATIC_RX_BUFFER_NUM
               - 24
               - 8
               - 6
               - 4
             * - WIFI_DYNAMIC_RX_BUFFER_NUM
               - 85
               - 64
               - 32
               - 32
             * - WIFI_STATIC_TX_BUFFER_NUM
               - 32
               - 32
               - 6
               - 4
             * - WIFI_RX_BA_WIN
               - 32
               - 16
               - 12
               - 禁用
             * - TCP_SND_BUF_DEFAULT (KB)
               - 85
               - 32
               - 32
               - 32
             * - TCP_WND_DEFAULT (KB)
               - 85
               - 32
               - 32
               - 32
             * - WIFI_IRAM_OPT
               - 开启
               - 开启
               - 开启
               - 关闭
             * - WIFI_RX_IRAM_OPT
               - 开启
               - 开启
               - 关闭
               - 关闭
             * - LWIP_IRAM_OPTIMIZATION
               - 开启
               - 关闭
               - 关闭
               - 关闭
             * - LWIP_UDP_RECVMBOX_SIZE
               - 16
               - 16
               - 16
               - 16
             * - INSTRUCTION_CACHE
               - 32
               - 16
               - 16
               - 16
             * - INSTRUCTION_CACHE_LINE
               - 32
               - 16
               - 16
               - 16
             * - INSTRUCTION_CACHE_WAYS
               - 8
               - 8
               - 8
               - 8
             * - DATA_CACHE
               - 64
               - 16
               - 16
               - 16
             * - DATA_CACHE_LINE
               - 32
               - 32
               - 32
               - 32
             * - DATA_CACHE_WAYS
               - 8
               - 8
               - 8
               - 8
             * - TCP 发送数据吞吐量 (Mbit/s)
               - 93.1
               - 62.5
               - 41.3
               - 42.7
             * - TCP 接收数据吞吐量 (Mbit/s)
               - 88.9
               - 46.5
               - 46.2
               - 37.9
             * - UDP 发送数据吞吐量 (Mbit/s)
               - 106.4
               - 106.2
               - 60.7
               - 50
             * - UDP 接收数据吞吐量 (Mbit/s)
               - 99.8
               - 92.6
               - 94.3
               - 53.3

        **八线 PSRAM：**

          .. list-table::
             :header-rows: 1
             :widths: 25 20 25 25 25

             * - 等级
               - Iperf
               - 默认
               - 节省内存
               - 最小
             * - 可用内存 (KB)
               - 49.1
               - 151.3
               - 215.3
               - 243.6
             * - WIFI_STATIC_RX_BUFFER_NUM
               - 24
               - 8
               - 6
               - 4
             * - WIFI_DYNAMIC_RX_BUFFER_NUM
               - 85
               - 64
               - 32
               - 32
             * - WIFI_STATIC_TX_BUFFER_NUM
               - 32
               - 32
               - 6
               - 4
             * - WIFI_RX_BA_WIN
               - 32
               - 16
               - 12
               - 禁用
             * - TCP_SND_BUF_DEFAULT (KB)
               - 85
               - 32
               - 32
               - 32
             * - TCP_WND_DEFAULT (KB)
               - 85
               - 32
               - 32
               - 32
             * - WIFI_IRAM_OPT
               - 开启
               - 开启
               - 开启
               - 关闭
             * - WIFI_RX_IRAM_OPT
               - 开启
               - 开启
               - 关闭
               - 关闭
             * - LWIP_IRAM_OPTIMIZATION
               - 开启
               - 关闭
               - 关闭
               - 关闭
             * - LWIP_UDP_RECVMBOX_SIZE
               - 16
               - 16
               - 16
               - 16
             * - INSTRUCTION_CACHE
               - 32
               - 16
               - 16
               - 16
             * - INSTRUCTION_CACHE_LINE
               - 32
               - 16
               - 16
               - 16
             * - INSTRUCTION_CACHE_WAYS
               - 8
               - 8
               - 8
               - 8
             * - DATA_CACHE
               - 64
               - 16
               - 16
               - 16
             * - DATA_CACHE_LINE
               - 32
               - 32
               - 32
               - 32
             * - DATA_CACHE_WAYS
               - 8
               - 8
               - 8
               - 8
             * - TCP 发送数据吞吐量 (Mbit/s)
               - 93.3
               - 58.4
               - 37.1
               - 35.6
             * - TCP 接收数据吞吐量 (Mbit/s)
               - 86.1
               - 43.6
               - 42.5
               - 35
             * - UDP 发送数据吞吐量 (Mbit/s)
               - 104.7
               - 82.2
               - 60.4
               - 47.9
             * - UDP 接收数据吞吐量 (Mbit/s)
               - 104.6
               - 104.8
               - 104
               - 55.7


{IDF_TARGET_NAME} Wi-Fi 节能模式
-----------------------------------------

本小节将简单介绍 Wi-Fi 节能模式相关的概念和使用方式，更加详细的介绍请参考 :doc:`低功耗模式使用指南 <../../api-guides/low-power-mode/index>`。

Station 睡眠
++++++++++++++++++++++

目前， {IDF_TARGET_NAME} Wi-Fi 支持 Modem-sleep 模式，该模式是 IEEE 802.11 协议中的传统节能模式。仅 station 模式支持该模式，且 station 必须先连接到 AP。如果使能了 Modem-sleep 模式，station 将定期在活动状态和睡眠状态之间切换。在睡眠状态下，RF、PHY 和 BB 处于关闭状态，以减少功耗。Modem-sleep 模式下，station 可以与 AP 保持连接。

Modem-sleep 模式包括最小和最大节能模式。在最小节能模式下，每个 DTIM 间隔，station 都将唤醒以接收 beacon。广播数据在 DTIM 之后传输，因此不会丢失。但是，由于 DTIM 间隔长短由 AP 决定，如果该间隔时间设置较短，则省电效果不大。

在最大节能模式下，每个监听间隔，station 都将唤醒以接收 beacon。可以设置该监听间隔长于 AP 的 DTIM 周期。在 DTIM 期间内，station 可能处于睡眠状态，广播数据会丢失。如果监听间隔较长，则可以节省更多电量，但广播数据更容易丢失。连接 AP 前，可以通过调用 API :cpp:func:`esp_wifi_set_config()` 配置监听间隔。

调用 :cpp:func:`esp_wifi_init()` 后，调用 ``esp_wifi_set_ps(WIFI_PS_MIN_MODEM)`` 可使能 Modem-sleep 最小节能模式。调用 ``esp_wifi_set_ps(WIFI_PS_MAX_MODEM)`` 可使能 Modem-sleep 最大节能模式。station 连接到 AP 时，Modem-sleep 模式将启动。station 与 AP 断开连接时，Modem-sleep 模式将停止。

调用 ``esp_wifi_set_ps(WIFI_PS_NONE)`` 可以完全禁用 Modem-sleep 模式。禁用会增大功耗，但可以最大限度减少实时接收 Wi-Fi 数据的延迟。使能 Modem-sleep 模式时，接收 Wi-Fi 数据的延迟时间可能与 DTIM 周期（最小节能模式）或监听间隔（最大节能模式）相同。

.. only:: SOC_SUPPORT_COEXISTENCE

    注意在共存模式下, 即使调用 ``esp_wifi_set_ps(WIFI_PS_NONE)``, Wi-Fi 也仅会在 Wi-Fi 时间片内保持活动状态, 非 Wi-Fi 时间片内仍然睡眠。请参照 :ref:`共存策略 <coexist_policy-cn>`。

默认的 Modem-sleep 模式是 WIFI_PS_MIN_MODEM。

AP 睡眠
+++++++++++++++++++++++++++++++

目前，{IDF_TARGET_NAME} AP 不支持 Wi-Fi 协议中定义的所有节能功能。具体来说，AP 只缓存所连 station 单播数据，不缓存组播数据。如果 {IDF_TARGET_NAME} AP 所连的 station 已使能节能功能，可能发生组播数据包丢失。

未来，{IDF_TARGET_NAME} AP 将支持所有节能功能。

非连接状态下的休眠
+++++++++++++++++++++++++++++++

非连接状态指的是 :cpp:func:`esp_wifi_start` 至 :cpp:func:`esp_wifi_stop` 期间内，没有建立 Wi-Fi 连接的阶段。

目前, {IDF_TARGET_NAME} Wi-Fi 支持以 station 模式运行时，在非连接状态下休眠。可以通过选项 :ref:`CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE` 配置该功能。

如果打开配置选项 :ref:`CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE`，则在该阶段内，RF, PHY and BB 将在空闲时被关闭，电流将会等同于 Modem-sleep 模式下的休眠电流。

配置选项 :ref:`CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE` 默认情况下将会被打开，共存模式下被 Menuconfig 强制打开。

.. _connectionless-module-power-save-cn:

非连接模块功耗管理
+++++++++++++++++++++++++++++++

非连接模块指的是一些不依赖于 Wi-Fi 连接的 Wi-Fi 模块，例如 ESP-NOW，DPP，FTM。这些模块从 :cpp:func:`esp_wifi_start` 开始工作至 :cpp:func:`esp_wifi_stop` 结束。

目前，ESP-NOW 以 station 模式工作时，既支持在连接状态下休眠，也支持在非连接状态下休眠。

非连接模块发包
*******************************

对于任何非连接模块，在开启了休眠的任何时间点都可以发包，不需要进行任何额外的配置。

此外，:cpp:func:`esp_wifi_80211_tx` 也在休眠时被支持。

非连接模块收包
*******************************

对于非连接模块，在开启休眠时如果需要进行收包，需要配置两个参数，分别为 `Window` 和 `Interval`。

在每个 `Interval` 开始时，RF, PHY and BB 将会被打开并保持 `Window` 的时间。非连接模块可以在此时间内收包。

**Interval**

 - 全局只有一个 `Interval` 参数，所有非连接模块共享它。其数值由 API :cpp:func:`esp_wifi_set_connectionless_interval` 配置，单位为毫秒。

 - `Interval` 的默认值为 `ESP_WIFI_CONNECTIONLESS_INTERVAL_DEFAULT_MODE` 。

 - 在 `Interval` 开始时，将会给出 :ref:`WIFI_EVENT_CONNECTIONLESS_MODULE_WAKE_INTERVAL_START <wifi-event-connectionless-module-wake-interval-start>` 事件，由于 `Window` 将在此时开始，可以在此事件内布置发包动作。

 - 在连接状态下，`Interval` 开始的时间点将会与 TBTT 时间点对齐。可以通过将非连接模块的接收端和发送端连接在同一路由器下，并在 :ref:`WIFI_EVENT_CONNECTIONLESS_MODULE_WAKE_INTERVAL_START <wifi-event-connectionless-module-wake-interval-start>` 事件内进行发包，以同步非连接模块的传输窗口，达到提高接收端收包成功率的效果。

 .. only:: esp32

    在 ESP32 上，TBTT 时间点会受到 DFS (Dynamic Frequency Scaling) 的干扰，如果想要在 ESP32 上通过 TBTT 同步非连接模块的传输窗口，需要禁用 DFS。

**Window**

 - 每个非连接模块在启动后都有其自身的 `Window` 参数，休眠模块将取所有模块 `Window` 的最大值运作。

 - 其数值由 API :cpp:func:`module_name_set_wake_window` 配置，单位为毫秒。

 - 模块 `Window` 的默认值为最大值。

.. table:: 不同 Window 与 Interval 组合下的 RF, PHY and BB 使用情况

    +----------------------+-----------------------------------------------------+---------------------------------------------------------------------------+
    |                      | Interval                                                                                                                        |
    +                      +-----------------------------------------------------+---------------------------------------------------------------------------+
    |                      | ``ESP_WIFI_CONNECTIONLESS_INTERVAL_DEFAULT_MODE``   | 1 - maximum                                                               |
    +--------+-------------+-----------------------------------------------------+---------------------------------------------------------------------------+
    | Window | 0           | not used                                                                                                                        |
    +        +-------------+-----------------------------------------------------+---------------------------------------------------------------------------+
    |        | 1 - maximum | default mode                                        | used periodically (Window < Interval) / used all time (Window ≥ Interval) |
    +--------+-------------+-----------------------------------------------------+---------------------------------------------------------------------------+

默认模式
*******************************

当 `Interval` 参数被配置为 `ESP_WIFI_CONNECTIONLESS_INTERVAL_DEFAULT_MODE` ，且有非零的 `Window` 参数时，非连接模块功耗管理将会按默认模式运行。

在没有与非 Wi-Fi 协议共存时，RF、PHY 和 BB 将会在默认模式下被一直打开。

在与非 Wi-Fi 协议共存时，RF、PHY 和 BB 资源被共存模块分时划给 Wi-Fi 非连接模块和非 Wi-Fi 协议使用。在默认模式下，Wi-Fi 非连接模块被允许周期性使用 RF、PHY 和 BB，并且具有稳定性能。

推荐在与非 Wi-Fi 协议共存时将非连接模块功耗管理配置为默认模式。
