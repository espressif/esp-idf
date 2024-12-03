Wi-Fi Performance and Power Save
======================================

:link_to_translation:`zh_CN:[中文]`

.. _wifi-buffer-usage:

Wi-Fi Buffer Usage
--------------------------

This section is only about the dynamic buffer configuration.

Why Buffer Configuration Is Important
+++++++++++++++++++++++++++++++++++++++

In order to get a high-performance system, consider the memory usage/configuration carefully for the following reasons:

 - the available memory in {IDF_TARGET_NAME} is limited.
 - currently, the default type of buffer in LwIP and Wi-Fi drivers is "dynamic", **which means that both the LwIP and Wi-Fi share memory with the application**. Programmers should always keep this in mind; otherwise, they will face a memory issue, such as "running out of heap memory".
 - it is very dangerous to run out of heap memory, as this will cause {IDF_TARGET_NAME} an "undefined behavior". Thus, enough heap memory should be reserved for the application, so that it never runs out of it.
 - the Wi-Fi throughput heavily depends on memory-related configurations, such as the TCP window size and Wi-Fi RX/TX dynamic buffer number.
 - the peak heap memory that the {IDF_TARGET_NAME} LwIP/Wi-Fi may consume depends on a number of factors, such as the maximum TCP/UDP connections that the application may have.
 - the total memory that the application requires is also an important factor when considering memory configuration.

Due to these reasons, there is not a good-for-all application configuration. Rather, it is recommended to consider memory configurations separately for every different application.

Dynamic vs. Static Buffer
++++++++++++++++++++++++++++++

The default type of buffer in Wi-Fi drivers is "dynamic". Most of the time the dynamic buffer can significantly save memory. However, it makes the application programming a little more difficult, because in this case the application needs to consider memory usage in Wi-Fi.

lwIP also allocates buffers at the TCP/IP layer, and this buffer allocation is also dynamic. See :ref:`lwIP documentation section about memory use and performance <lwip-performance>`.

Peak Wi-Fi Dynamic Buffer
++++++++++++++++++++++++++++++

The Wi-Fi driver supports several types of buffer (refer to :ref:`wifi-buffer-configure`). However, this section is about the usage of the dynamic Wi-Fi buffer only.
The peak heap memory that Wi-Fi consumes is the **theoretically-maximum memory** that the Wi-Fi driver consumes. Generally, the peak memory depends on:

- :math:`b_{rx}` the number of dynamic RX buffers that are configured
- :math:`b_{tx}` the number of dynamic TX buffers that are configured
- :math:`m_{rx}` the maximum packet size that the Wi-Fi driver can receive
- :math:`m_{tx}` the maximum packet size that the Wi-Fi driver can send

So, the peak memory that the Wi-Fi driver consumes (:math:`p`) can be calculated with the following formula:

.. math::

    p = (b_{rx} * m_{rx}) + (b_{tx} * m_{tx})

Generally, the dynamic TX long buffers and dynamic TX long long buffers can be ignored, because they are management frames which only have a small impact on the system.


{IDF_TARGET_NAME} Wi-Fi Throughput
-----------------------------------

The table below shows the best throughput results gained in Espressif's lab and in a shielded box.

.. only:: esp32

    .. list-table::
       :header-rows: 1
       :widths: 10 10 10 10 25

       * - Type/Throughput
         - Air In Lab
         - Shield-box
         - Test Tool
         - IDF Version (commit ID)
       * - Raw 802.11 Packet RX
         - N/A
         - **130 MBit/s**
         - Internal tool
         - NA
       * - Raw 802.11 Packet TX
         - N/A
         - **130 MBit/s**
         - Internal tool
         - NA
       * - UDP RX
         - 30 MBit/s
         - 85 MBit/s
         - iperf example
         - 15575346
       * - UDP TX
         - 30 MBit/s
         - 75 MBit/s
         - iperf example
         - 15575346
       * - TCP RX
         - 20 MBit/s
         - 65 MBit/s
         - iperf example
         - 15575346
       * - TCP TX
         - 20 MBit/s
         - 75 MBit/s
         - iperf example
         - 15575346

    When the throughput is tested by iperf example, the sdkconfig is :idf_file:`examples/wifi/iperf/sdkconfig.defaults.esp32`.

.. only:: esp32s2

    .. list-table::
       :header-rows: 1
       :widths: 10 10 10 10 25

       * - Type/Throughput
         - Air In Lab
         - Shield-box
         - Test Tool
         - IDF Version (commit ID)
       * - Raw 802.11 Packet RX
         - N/A
         - **130 MBit/s**
         - Internal tool
         - NA
       * - Raw 802.11 Packet TX
         - N/A
         - **130 MBit/s**
         - Internal tool
         - NA
       * - UDP RX
         - 30 MBit/s
         - 70 MBit/s
         - iperf example
         - 15575346
       * - UDP TX
         - 30 MBit/s
         - 50 MBit/s
         - iperf example
         - 15575346
       * - TCP RX
         - 20 MBit/s
         - 32 MBit/s
         - iperf example
         - 15575346
       * - TCP TX
         - 20 MBit/s
         - 37 MBit/s
         - iperf example
         - 15575346

    When the throughput is tested by iperf example, the sdkconfig is :idf_file:`examples/wifi/iperf/sdkconfig.defaults.esp32s2`.

.. only:: esp32c3

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15 20

        * - Type/Throughput
          - Air In Lab
          - Shield-box
          - Test Tool
          - IDF Version (commit ID)
        * - Raw 802.11 Packet RX
          - N/A
          - **130 MBit/s**
          - Internal tool
          - NA
        * - Raw 802.11 Packet TX
          - N/A
          - **130 MBit/s**
          - Internal tool
          - NA
        * - UDP RX
          - 30 MBit/s
          - 50 MBit/s
          - iperf example
          - 15575346
        * - UDP TX
          - 30 MBit/s
          - 40 MBit/s
          - iperf example
          - 15575346
        * - TCP RX
          - 20 MBit/s
          - 35 MBit/s
          - iperf example
          - 15575346
        * - TCP TX
          - 20 MBit/s
          - 37 MBit/s
          - iperf example
          - 15575346

    When the throughput is tested by iperf example, the sdkconfig is :idf_file:`examples/wifi/iperf/sdkconfig.defaults.esp32c3`.

.. only:: esp32c5

    - 2.4 GHz band

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15 20

        * - Type/Throughput
          - Air In Lab
          - Shield-box
          - Test Tool
          - IDF Version (commit ID)
        * - Raw 802.11 Packet RX
          - N/A
          - **130 MBit/s**
          - Internal tool
          - NA
        * - Raw 802.11 Packet TX
          - N/A
          - **130 MBit/s**
          - Internal tool
          - NA
        * - UDP RX
          - 30 MBit/s
          - 68 MBit/s
          - iperf example
          - 7ff0a07d
        * - UDP TX
          - 30 MBit/s
          - 63 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP RX
          - 20 MBit/s
          - 59 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP TX
          - 20 MBit/s
          - 49 MBit/s
          - iperf example
          - 7ff0a07d

    - 5 GHz band

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15 20

        * - Type/Throughput
          - Air In Lab
          - Shield-box
          - Test Tool
          - IDF Version (commit ID)
        * - Raw 802.11 Packet RX
          - N/A
          - **130 MBit/s**
          - Internal tool
          - NA
        * - Raw 802.11 Packet TX
          - N/A
          - **130 MBit/s**
          - Internal tool
          - NA
        * - UDP RX
          - 30 MBit/s
          - 71 MBit/s
          - iperf example
          - 7ff0a07d
        * - UDP TX
          - 30 MBit/s
          - 64 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP RX
          - 20 MBit/s
          - 61 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP TX
          - 20 MBit/s
          - 50 MBit/s
          - iperf example
          - 7ff0a07d

    When the throughput is tested by iperf example, the sdkconfig is :idf_file:`examples/wifi/iperf/sdkconfig.defaults.esp32c5`.

.. only:: esp32c6

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15 20

        * - Type/Throughput
          - Air In Lab
          - Shield-box
          - Test Tool
          - IDF Version (commit ID)
        * - Raw 802.11 Packet RX
          - N/A
          - **130 MBit/s**
          - Internal tool
          - NA
        * - Raw 802.11 Packet TX
          - N/A
          - **130 MBit/s**
          - Internal tool
          - NA
        * - UDP RX
          - 30 MBit/s
          - 63 MBit/s
          - iperf example
          - 7ff0a07d
        * - UDP TX
          - 30 MBit/s
          - 51 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP RX
          - 20 MBit/s
          - 46 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP TX
          - 20 MBit/s
          - 43 MBit/s
          - iperf example
          - 7ff0a07d

    When the throughput is tested by iperf example, the sdkconfig is :idf_file:`examples/wifi/iperf/sdkconfig.defaults.esp32c6`.

.. only:: esp32c61

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15 20

        * - Type/Throughput
          - Air In Lab
          - Shield-box
          - Test Tool
          - IDF Version (commit ID)
        * - Raw 802.11 Packet RX
          - N/A
          - **130 MBit/s**
          - Internal tool
          - NA
        * - Raw 802.11 Packet TX
          - N/A
          - **130 MBit/s**
          - Internal tool
          - NA
        * - UDP RX
          - 30 MBit/s
          - 68 MBit/s
          - iperf example
          - 7ff0a07d
        * - UDP TX
          - 30 MBit/s
          - 53 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP RX
          - 20 MBit/s
          - 45 MBit/s
          - iperf example
          - 7ff0a07d
        * - TCP TX
          - 20 MBit/s
          - 37 MBit/s
          - iperf example
          - 7ff0a07d

    When the throughput is tested by iperf example, the sdkconfig is :idf_file:`examples/wifi/iperf/sdkconfig.defaults.esp32c61`.

.. only:: esp32s3

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15 20

        * - Type/Throughput
          - Air In Lab
          - Shield-box
          - Test Tool
          - IDF Version (commit ID)
        * - Raw 802.11 Packet RX
          - N/A
          - **130 MBit/s**
          - Internal tool
          - NA
        * - Raw 802.11 Packet TX
          - N/A
          - **130 MBit/s**
          - Internal tool
          - NA
        * - UDP RX
          - 30 MBit/s
          - 88 MBit/s
          - iperf example
          - 15575346
        * - UDP TX
          - 30 MBit/s
          - 98 MBit/s
          - iperf example
          - 15575346
        * - TCP RX
          - 20 MBit/s
          - 73 MBit/s
          - iperf example
          - 15575346
        * - TCP TX
          - 20 MBit/s
          - 83 MBit/s
          - iperf example
          - 15575346

    When the throughput is tested by iperf example, the sdkconfig is :idf_file:`examples/wifi/iperf/sdkconfig.defaults.esp32s3`.


.. _How-to-improve-Wi-Fi-performance:

How to Improve Wi-Fi Performance
----------------------------------

The performance of {IDF_TARGET_NAME} Wi-Fi is affected by many parameters, and there are mutual constraints between each parameter. A proper configuration cannot only improve performance, but also increase available memory for applications and improve stability.

This section briefly explains the operating mode of the Wi-Fi/LwIP protocol stack and the role of each parameter. It also gives several recommended configuration ranks to help choose the appropriate rank according to the usage scenario.

Protocol Stack Operation Mode
++++++++++++++++++++++++++++++++++

.. figure:: ../../../_static/api-guides-WiFi-driver-how-to-improve-WiFi-performance.png
    :align: center

    {IDF_TARGET_NAME} datapath

The {IDF_TARGET_NAME} protocol stack is divided into four layers: Application, LwIP, Wi-Fi, and Hardware.

 - During receiving, hardware puts the received packet into DMA buffer, and then transfers it into the RX buffer of Wi-Fi and LwIP in turn for related protocol processing, and finally to the application layer. The Wi-Fi RX buffer and the LwIP RX buffer shares the same buffer by default. In other words, the Wi-Fi forwards the packet to LwIP by reference by default.

 - During sending, the application copies the messages to be sent into the TX buffer of the LwIP layer for TCP/IP encapsulation. The messages will then be passed to the TX buffer of the Wi-Fi layer for MAC encapsulation and wait to be sent.

Parameters
++++++++++++++

Increasing the size or number of the buffers mentioned above properly can improve Wi-Fi performance. Meanwhile, it will reduce available memory to the application. The following is an introduction to the parameters that users need to configure:

**RX direction:**

 - :ref:`CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM`
    This parameter indicates the number of DMA buffer at the hardware layer. Increasing this parameter will increase the sender's one-time receiving throughput, thereby improving the Wi-Fi protocol stack ability to handle burst traffic.

 - :ref:`CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM`
    This parameter indicates the number of RX buffer in the Wi-Fi layer. Increasing this parameter will improve the performance of packet reception. This parameter needs to match the RX buffer size of the LwIP layer.

 - :ref:`CONFIG_ESP_WIFI_RX_BA_WIN`
    This parameter indicates the size of the AMPDU BA Window at the receiving end. This parameter should be configured to the smaller value between twice of :ref:`CONFIG_ESP_WIFI_STATIC_RX_BUFFER_NUM` and :ref:`CONFIG_ESP_WIFI_DYNAMIC_RX_BUFFER_NUM`.

 - :ref:`CONFIG_LWIP_TCP_WND_DEFAULT`
    This parameter represents the RX buffer size of the LwIP layer for each TCP stream. Its value should be configured to the value of WIFI_DYNAMIC_RX_BUFFER_NUM (KB) to reach a high and stable performance. Meanwhile, in case of multiple streams, this value needs to be reduced proportionally.

**TX direction:**

 - :ref:`CONFIG_ESP_WIFI_TX_BUFFER`
    This parameter indicates the type of TX buffer, it is recommended to configure it as a dynamic buffer, which can make full use of memory.

 - :ref:`CONFIG_ESP_WIFI_DYNAMIC_TX_BUFFER_NUM`
    This parameter indicates the number of TX buffer on the Wi-Fi layer. Increasing this parameter will improve the performance of packet sending. The parameter value needs to match the TX buffer size of the LwIP layer.

 - :ref:`CONFIG_LWIP_TCP_SND_BUF_DEFAULT`
    This parameter represents the TX buffer size of the LwIP layer for each TCP stream. Its value should be configured to the value of WIFI_DYNAMIC_TX_BUFFER_NUM (KB) to reach a high and stable performance. In case of multiple streams, this value needs to be reduced proportionally.

**Throughput optimization by placing code in IRAM:**

.. only:: esp32 or esp32s2

    - :ref:`CONFIG_ESP_WIFI_IRAM_OPT`
        If this option is enabled, some Wi-Fi functions are moved to IRAM, improving throughput. This increases IRAM usage by 15 kB.

    - :ref:`CONFIG_ESP_WIFI_RX_IRAM_OPT`
        If this option is enabled, some Wi-Fi RX functions are moved to IRAM, improving throughput. This increases IRAM usage by 16 kB.

 - :ref:`CONFIG_LWIP_IRAM_OPTIMIZATION`
    If this option is enabled, some LwIP functions are moved to IRAM, improving throughput. This increases IRAM usage by 13 kB.

.. only:: esp32c6

    - :ref:`CONFIG_ESP_WIFI_IRAM_OPT`
        If this option is enabled, some Wi-Fi functions are moved to IRAM, improving throughput. This increases IRAM usage by 13 kB.

    - :ref:`CONFIG_ESP_WIFI_RX_IRAM_OPT`
        If this option is enabled, some Wi-Fi RX functions are moved to IRAM, improving throughput. This increases IRAM usage by 7 kB.

    - :ref:`CONFIG_LWIP_IRAM_OPTIMIZATION`
        If this option is enabled, some LwIP functions are moved to IRAM, improving throughput. This increases IRAM usage by 14 kB.

.. only:: esp32s2

    **CACHE:**

     - :ref:`CONFIG_ESP32S2_INSTRUCTION_CACHE_SIZE`
        Configure the size of the instruction Cache.

     - :ref:`CONFIG_ESP32S2_INSTRUCTION_CACHE_LINE_SIZE`
        Configure the width of the instruction Cache bus.

.. only:: esp32s3

    **CACHE:**

     - :ref:`CONFIG_ESP32S3_INSTRUCTION_CACHE_SIZE`
        Configure the size of the instruction Cache.

     - :ref:`CONFIG_ESP32S3_INSTRUCTION_CACHE_LINE_SIZE`
        Configure the size of the instruction Cache bus.

     - :ref:`CONFIG_ESP32S3_ICACHE_ASSOCIATED_WAYS`
        Configure the associated ways of the instruction Cache.

     - :ref:`CONFIG_ESP32S3_DATA_CACHE_SIZE`
        Configure the size of the Data Cache.

     - :ref:`CONFIG_ESP32S3_DATA_CACHE_LINE_SIZE`
        Configure the line size of the Data Cache.

     - :ref:`CONFIG_ESP32S3_DCACHE_ASSOCIATED_WAYS`
        Configure the associated ways of the Data Cache.

.. note::
    The buffer size mentioned above is fixed as 1.6 KB.

How to Configure Parameters
++++++++++++++++++++++++++++

The memory of {IDF_TARGET_NAME} is shared by protocol stack and applications.

Here, several configuration ranks are given. In most cases, the user should select a suitable rank for parameter configuration according to the size of the memory occupied by the application.

The parameters not mentioned in the following table should be set to the default.

.. only:: esp32

     .. list-table::
        :header-rows: 1
        :widths: 10 5 5 10 5 5 10 5

        * - Rank
          - Iperf
          - TX prior
          - High-performance
          - RX prior
          - Default
          - Memory saving
          - Minimum
        * - Available memory (KB)
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
          - Disable
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
          - ENABLE
          - ENABLE
          - ENABLE
          - ENABLE
          - ENABLE
          - ENABLE
          - ENABLE
        * - WIFI_RX_IRAM_OPT
          - ENABLE
          - ENABLE
          - ENABLE
          - ENABLE
          - ENABLE
          - ENABLE
          - ENABLE
        * - LWIP_IRAM_OPTIMIZATION
          - ENABLE
          - ENABLE
          - ENABLE
          - ENABLE
          - ENABLE
          - ENABLE
          - ENABLE
        * - TCP TX throughput (Mbit/s)
          - 74.6
          - 50.8
          - 46.5
          - 39.9
          - 44.2
          - 33.8
          - 25.6
        * - TCP RX throughput (Mbit/s)
          - 63.6
          - 35.5
          - 42.3
          - 48.5
          - 40.5
          - 30.1
          - 27.8
        * - UDP TX throughput (Mbit/s)
          - 76.2
          - 75.1
          - 74.1
          - 72.4
          - 69.6
          - 64.1
          - 36.5
        * - UDP RX throughput (Mbit/s)
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

        * - Rank
          - Iperf
          - High-performance
          - Default
          - Memory saving
          - Minimum
        * - Available memory (KB)
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
          - ENABLE
          - ENABLE
          - ENABLE
          - ENABLE
          - DISABLE
        * - WIFI_RX_IRAM_OPT
          - ENABLE
          - ENABLE
          - ENABLE
          - DISABLE
          - DISABLE
        * - LWIP_IRAM_OPTIMIZATION
          - ENABLE
          - ENABLE
          - DISABLE
          - DISABLE
          - DISABLE
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
        * - TCP TX throughput (Mbit/s)
          - 37.6
          - 33.1
          - 22.5
          - 12.2
          - 5.5
        * - TCP RX throughput (Mbit/s)
          - 31.5
          - 28.1
          - 20.1
          - 13.1
          - 7.2
        * - UDP TX throughput (Mbit/s)
          - 58.1
          - 57.3
          - 28.1
          - 22.6
          - 8.7
        * - UDP RX throughput (Mbit/s)
          - 78.1
          - 66.7
          - 65.3
          - 53.8
          - 28.5

.. only:: esp32c3

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15

        * - Rank
          - Iperf
          - Default
          - Minimum
        * - Available memory (KB)
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
          - ENABLE
          - ENABLE
          - DISABLE
        * - TCP TX throughput (Mbit/s)
          - 38.1
          - 27.2
          - 20.4
        * - TCP RX throughput (Mbit/s)
          - 35.3
          - 24.2
          - 17.4
        * - UDP TX throughput (Mbit/s)
          - 40.6
          - 38.9
          - 34.1
        * - UDP RX throughput (Mbit/s)
          - 52.4
          - 44.5
          - 44.2

.. only:: esp32c6

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15

        * - Rank
          - Iperf
          - Default
          - Minimum
        * - Available memory (KB)
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
          - ENABLE
          - ENABLE
          - DISABLE
        * - TCP TX throughput (Mbit/s)
          - 30.5
          - 25.9
          - 16.4
        * - TCP RX throughput (Mbit/s)
          - 27.8
          - 21.6
          - 14.3
        * - UDP TX throughput (Mbit/s)
          - 37.8
          - 36.1
          - 34.6
        * - UDP RX throughput (Mbit/s)
          - 41.5
          - 36.8
          - 36.7

.. only:: esp32c2

     .. list-table::
        :header-rows: 1
        :widths: 10 10 10 15

        * - Rank
          - Iperf
          - Default
          - Minimum
        * - Available memory (KB)
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
          - ENABLE
          - ENABLE
          - DISABLE
        * - TCP TX throughput (Mbit/s)
          - 21.6
          - 21.4
          - 14.3
        * - TCP RX throughput (Mbit/s)
          - 19.1
          - 17.9
          - 12.4
        * - UDP TX throughput (Mbit/s)
          - 26.4
          - 26.3
          - 25.0
        * - UDP RX throughput (Mbit/s)
          - 32.3
          - 31.5
          - 27.7

.. only:: esp32s3

     .. list-table::
        :header-rows: 1
        :widths: 25 20 25 25

        * - Rank
          - Iperf
          - Default
          - Minimum
        * - Available memory (KB)
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
          - ENABLE
          - ENABLE
          - ENABLE
        * - WIFI_RX_IRAM_OPT
          - ENABLE
          - ENABLE
          - ENABLE
        * - LWIP_IRAM_OPTIMIZATION
          - ENABLE
          - ENABLE
          - DISABLE
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
        * - TCP TX throughput (Mbit/s)
          - 83.93
          - 64.28
          - 23.17
        * - TCP RX throughput (Mbit/s)
          - 73.98
          - 60.39
          - 18.11
        * - UDP TX throughput (Mbit/s)
          - 98.69
          - 96.28
          - 48.78
        * - UDP RX throughput (Mbit/s)
          - 88.58
          - 86.57
          - 59.45

.. only:: esp32 or esp32s3

    .. note::
        The test was performed with a single stream in a shielded box using an ASUS RT-N66U router.
        {IDF_TARGET_NAME}'s CPU is dual core with 240 MHz. {IDF_TARGET_NAME}'s flash is in QIO mode with 80 MHz.

.. only:: esp32s2

    .. note::
        The test was performed with a single stream in a shielded box using an ASUS RT-N66U router.
        {IDF_TARGET_NAME}'s CPU is single core with 240 MHz. {IDF_TARGET_NAME}'s flash is in QIO mode with 80 MHz.

.. only:: esp32c3

    .. note::
        The test was performed with a single stream in a shielded box using an ASUS RT-N66U router.
        {IDF_TARGET_NAME}'s CPU is single core with 160 MHz. {IDF_TARGET_NAME}'s flash is in QIO mode with 80 MHz.

.. only:: esp32c6

    .. note::
        The test was performed with a single stream in a shielded box using an XIAOMI AX-6000 router.
        {IDF_TARGET_NAME}'s CPU is single core with 160 MHz. {IDF_TARGET_NAME}'s flash is in QIO mode with 80 MHz.

.. only:: esp32c2

    .. note::
        The test was performed with a single stream in a shielded box using an Redmi RM2100 router.
        {IDF_TARGET_NAME}'s CPU is single core with 120 MHz. {IDF_TARGET_NAME}'s flash is in QIO mode with 60 MHz.

.. only:: esp32

    **Ranks:**

     - **Iperf rank**
        {IDF_TARGET_NAME} extreme performance rank used to test extreme performance.

     - **High-performance rank**
        The {IDF_TARGET_NAME}'s high-performance configuration rank, suitable for scenarios where the application occupies less memory and has high-performance requirements. In this rank, users can choose to use the RX prior rank or the TX prior rank according to the usage scenario.

     - **Default rank**
        {IDF_TARGET_NAME}'s default configuration rank, the available memory, and performance are in balance.

     - **Memory saving rank**
        This rank is suitable for scenarios where the application requires a large amount of memory, and the transceiver performance will be reduced in this rank.

     - **Minimum rank**
        This is the minimum configuration rank of {IDF_TARGET_NAME}. The protocol stack only uses the necessary memory for running. It is suitable for scenarios where there is no requirement for performance and the application requires lots of space.

.. only:: esp32s2

    **Ranks:**

     - **Iperf rank**
        {IDF_TARGET_NAME} extreme performance rank used to test extreme performance.

     - **High-performance rank**
        The {IDF_TARGET_NAME}'s high-performance configuration rank, suitable for scenarios where the application occupies less memory and has high-performance requirements.

     - **Default rank**
        {IDF_TARGET_NAME}'s default configuration rank, the available memory, and performance are in balance.

     - **Memory saving rank**
        This rank is suitable for scenarios where the application requires a large amount of memory, and the transceiver performance will be reduced in this rank.

     - **Minimum rank**
        This is the minimum configuration rank of {IDF_TARGET_NAME}. The protocol stack only uses the necessary memory for running. It is suitable for scenarios where there is no requirement for performance and the application requires lots of space.

.. only:: esp32c3 or esp32s3 or esp32c6

    **Ranks:**

     - **Iperf rank**
        {IDF_TARGET_NAME} extreme performance rank used to test extreme performance.

     - **Default rank**
        {IDF_TARGET_NAME}'s default configuration rank, the available memory, and performance are in balance.

     - **Minimum rank**
        This is the minimum configuration rank of {IDF_TARGET_NAME}. The protocol stack only uses the necessary memory for running. It is suitable for scenarios where there is no requirement for performance and the application requires lots of space.

.. only:: SOC_SPIRAM_SUPPORTED

    Using PSRAM
    ++++++++++++++++++++++++++++

    PSRAM is generally used when the application takes up a lot of memory. In this mode, the :ref:`CONFIG_ESP_WIFI_TX_BUFFER` is forced to be static. :ref:`CONFIG_ESP_WIFI_STATIC_TX_BUFFER_NUM` indicates the number of DMA buffers at the hardware layer, and increasing this parameter can improve performance.
    The following are the recommended ranks for using PSRAM:

    .. only:: esp32

        .. list-table::
             :header-rows: 1
             :widths: 15 10 10 15 10

             * - Rank
               - Iperf
               - Default
               - Memory saving
               - Minimum
             * - Available memory (KB)
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
               - Disable
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
               - ENABLE
               - ENABLE
               - ENABLE
               - DISABLE
             * - WIFI_RX_IRAM_OPT
               - ENABLE
               - ENABLE
               - DISABLE
               - DISABLE
             * - LWIP_IRAM_OPTIMIZATION
               - ENABLE
               - DISABLE
               - DISABLE
               - DISABLE
             * - TCP TX throughput (Mbit/s)
               - 37.5
               - 31.7
               - 21.7
               - 14.6
             * - TCP RX throughput (Mbit/s)
               - 31.5
               - 29.8
               - 26.5
               - 21.1
             * - UDP TX throughput (Mbit/s)
               - 69.1
               - 31.5
               - 27.1
               - 24.1
             * - UDP RX throughput (Mbit/s)
               - 40.1
               - 38.5
               - 37.5
               - 36.9

    .. only:: esp32s2

        .. list-table::
             :header-rows: 1
             :widths: 10 10 10 10 15

             * - Rank
               - Iperf
               - Default
               - Memory saving
               - Minimum
             * - Available memory (KB)
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
               - Disable
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
               - ENABLE
               - ENABLE
               - ENABLE
               - DISABLE
             * - WIFI_RX_IRAM_OPT
               - ENABLE
               - ENABLE
               - DISABLE
               - DISABLE
             * - LWIP_IRAM_OPTIMIZATION
               - ENABLE
               - DISABLE
               - DISABLE
               - DISABLE
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
             * - TCP TX throughput (Mbit/s)
               - 40.1
               - 29.2
               - 20.1
               - 8.9
             * - TCP RX throughput (Mbit/s)
               - 21.9
               - 16.8
               - 14.8
               - 9.6
             * - UDP TX throughput (Mbit/s)
               - 50.1
               - 25.7
               - 22.4
               - 10.2
             * - UDP RX throughput (Mbit/s)
               - 45.3
               - 43.1
               - 28.5
               - 15.1

        .. note::
            Reaching peak performance may cause task watchdog. It is a normal phenomenon considering the CPU may have no time for lower priority tasks.

    .. only:: esp32s3

        **PSRAM with 4 lines:**

          .. list-table::
             :header-rows: 1
             :widths: 25 20 25 25 25

             * - Rank
               - Iperf
               - Default
               - Memory saving
               - Minimum
             * - Available memory (KB)
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
               - Disable
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
               - ENABLE
               - ENABLE
               - ENABLE
               - DISABLE
             * - WIFI_RX_IRAM_OPT
               - ENABLE
               - ENABLE
               - DISABLE
               - DISABLE
             * - LWIP_IRAM_OPTIMIZATION
               - ENABLE
               - DISABLE
               - DISABLE
               - DISABLE
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
             * - TCP TX throughput (Mbit/s)
               - 93.1
               - 62.5
               - 41.3
               - 42.7
             * - TCP RX throughput (Mbit/s)
               - 88.9
               - 46.5
               - 46.2
               - 37.9
             * - UDP TX throughput (Mbit/s)
               - 106.4
               - 106.2
               - 60.7
               - 50
             * - UDP RX throughput (Mbit/s)
               - 99.8
               - 92.6
               - 94.3
               - 53.3

        **PSRAM with 8 lines:**

          .. list-table::
             :header-rows: 1
             :widths: 25 20 25 25 25

             * - Rank
               - Iperf
               - Default
               - Memory saving
               - Minimum
             * - Available memory (KB)
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
               - Disable
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
               - ENABLE
               - ENABLE
               - ENABLE
               - DISABLE
             * - WIFI_RX_IRAM_OPT
               - ENABLE
               - ENABLE
               - DISABLE
               - DISABLE
             * - LWIP_IRAM_OPTIMIZATION
               - ENABLE
               - DISABLE
               - DISABLE
               - DISABLE
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
             * - TCP TX throughput (Mbit/s)
               - 93.3
               - 58.4
               - 37.1
               - 35.6
             * - TCP RX throughput (Mbit/s)
               - 86.1
               - 43.6
               - 42.5
               - 35
             * - UDP TX throughput (Mbit/s)
               - 104.7
               - 82.2
               - 60.4
               - 47.9
             * - UDP RX throughput (Mbit/s)
               - 104.6
               - 104.8
               - 104
               - 55.7


{IDF_TARGET_NAME} Wi-Fi Power-saving Mode
-----------------------------------------

This subsection will briefly introduce the concepts and usage related to Wi-Fi Power Saving Mode, for a more detailed introduction please refer to the :doc:`Low Power Mode User Guide <../../api-guides/low-power-mode/index>`.

Station Sleep
++++++++++++++++++++++

Currently, {IDF_TARGET_NAME} Wi-Fi supports the Modem-sleep mode which refers to the legacy power-saving mode in the IEEE 802.11 protocol. Modem-sleep mode works in station-only mode and the station must connect to the AP first. If the Modem-sleep mode is enabled, station will switch between active and sleep state periodically. In sleep state, RF, PHY and BB are turned off in order to reduce power consumption. Station can keep connection with AP in modem-sleep mode.

Modem-sleep mode includes minimum and maximum power-saving modes. In minimum power-saving mode, station wakes up every DTIM to receive beacon. Broadcast data will not be lost because it is transmitted after DTIM. However, it cannot save much more power if DTIM is short for DTIM is determined by AP.

In maximum power-saving mode, station wakes up in every listen interval to receive beacon. This listen interval can be set to be longer than the AP DTIM period. Broadcast data may be lost because station may be in sleep state at DTIM time. If listen interval is longer, more power is saved, but broadcast data is more easy to lose. Listen interval can be configured by calling API :cpp:func:`esp_wifi_set_config()` before connecting to AP.

Call ``esp_wifi_set_ps(WIFI_PS_MIN_MODEM)`` to enable Modem-sleep minimum power-saving mode or ``esp_wifi_set_ps(WIFI_PS_MAX_MODEM)`` to enable Modem-sleep maximum power-saving mode after calling :cpp:func:`esp_wifi_init()`. When station connects to AP, Modem-sleep will start. When station disconnects from AP, Modem-sleep will stop.

Call ``esp_wifi_set_ps(WIFI_PS_NONE)`` to disable Modem-sleep mode entirely. Disabling it increases power consumption, but minimizes the delay in receiving Wi-Fi data in real time. When Modem-sleep mode is enabled, the delay in receiving Wi-Fi data may be the same as the DTIM cycle (minimum power-saving mode) or the listening interval (maximum power-saving mode).

.. only:: SOC_SUPPORT_COEXISTENCE

    Note that in coexist mode, Wi-Fi will remain active only during Wi-Fi time slice, and sleep during non Wi-Fi time slice even if ``esp_wifi_set_ps(WIFI_PS_NONE)`` is called. Please refer to :ref:`coexist policy <coexist_policy>`.

The default Modem-sleep mode is WIFI_PS_MIN_MODEM.

AP Sleep
+++++++++++++++++++++++++++++++

Currently, {IDF_TARGET_NAME} AP does not support all of the power-saving feature defined in Wi-Fi specification. To be specific, the AP only caches unicast data for the stations connect to this AP, but does not cache the multicast data for the stations. If stations connected to the {IDF_TARGET_NAME} AP are power-saving enabled, they may experience multicast packet loss.

In the future, all power-saving features will be supported on {IDF_TARGET_NAME} AP.

Disconnected State Sleep
+++++++++++++++++++++++++++++++

Disconnected state is the duration without Wi-Fi connection between :cpp:func:`esp_wifi_start` to :cpp:func:`esp_wifi_stop`.

Currently, {IDF_TARGET_NAME} Wi-Fi supports sleep mode in disconnected state if running at station mode. This feature could be configured by Menuconfig choice :ref:`CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE`.

If :ref:`CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE` is enabled, RF, PHY and BB would be turned off in disconnected state when IDLE. The current would be same with current at modem-sleep.

The choice :ref:`CONFIG_ESP_WIFI_STA_DISCONNECTED_PM_ENABLE` would be selected by default, while it would be selected forcefully in Menuconfig at coexistence mode.

.. _connectionless-module-power-save:

Connectionless Modules Power-saving
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Connectionless modules are those Wi-Fi modules not relying on Wi-Fi connection, e.g ESP-NOW, DPP, FTM. These modules start from :cpp:func:`esp_wifi_start`, working until :cpp:func:`esp_wifi_stop`.

Currently, if ESP-NOW works at station mode, its supported to sleep at both connected state and disconnected state.

Connectionless Modules TX
*******************************

For each connectionless module, its supported to TX at any sleeping time without any extra configuration.

Meanwhile, :cpp:func:`esp_wifi_80211_tx` is supported at sleep as well.

Connectionless Modules RX
*******************************

For each connectionless module, two parameters shall be configured to RX at sleep, which are `Window` and `Interval`.

At the start of `Interval` time, RF, PHY, BB would be turned on and kept for `Window` time. Connectionless Module could RX in the duration.

**Interval**

 - There is only one `Interval`. Its configured by :cpp:func:`esp_wifi_set_connectionless_interval`. The unit is milliseconds.

 - The default value of `Interval` is `ESP_WIFI_CONNECTIONLESS_INTERVAL_DEFAULT_MODE`.

 - Event :ref:`wifi-event-connectionless-module-wake-interval-start` would be posted at the start of `Interval`. Since `Window` also starts at that moment, its recommended to TX in that event.

 - At connected state, the start of `Interval` would be aligned with TBTT. To improve the packet reception success rate in connectionless modules, the sender and receiver can be connected to the same AP, and packets can be transmitted within the event :ref:`wifi-event-connectionless-module-wake-interval-start`. This synchronization helps align the connectionless modules transmission window.

 .. only:: esp32

    On the ESP32, TBTT timing is affected by DFS(Dynamic Frequency Scaling). To synchronize the connectionless modules transmission window using TBTT on the ESP32, DFS must be disabled.

**Window**

 - Each connectionless module has its own `Window` after start. Connectionless Modules Power-saving would work with the max one among them.

 - `Window` is configured by :cpp:func:`module_name_set_wake_window`. The unit is milliseconds.

 - The default value of `Window` is the maximum.

.. table:: RF, PHY and BB usage under different circumstances

    +----------------------+-----------------------------------------------------+---------------------------------------------------------------------------+
    |                      | Interval                                                                                                                        |
    +                      +-----------------------------------------------------+---------------------------------------------------------------------------+
    |                      | ``ESP_WIFI_CONNECTIONLESS_INTERVAL_DEFAULT_MODE``   | 1 - maximum                                                               |
    +--------+-------------+-----------------------------------------------------+---------------------------------------------------------------------------+
    | Window | 0           | not used                                                                                                                        |
    +        +-------------+-----------------------------------------------------+---------------------------------------------------------------------------+
    |        | 1 - maximum | default mode                                        | used periodically (Window < Interval) / used all time (Window ≥ Interval) |
    +--------+-------------+-----------------------------------------------------+---------------------------------------------------------------------------+

Default Mode
*******************************

If `Interval` is ``ESP_WIFI_CONNECTIONLESS_INTERVAL_DEFAULT_MODE`` with non-zero `Window`, Connectionless Modules Power-saving would work in default mode.

In default mode, RF, PHY, BB would be kept on if no coexistence with non-Wi-Fi protocol.

With coexistence, RF, PHY, BB resources are allocated by coexistence module to Wi-Fi connectionless module and non-Wi-Fi module, using time-division method. In default mode, Wi-Fi connectionless module is allowed to use RF, BB, PHY periodically under a stable performance.

Its recommended to configure Connectionless Modules Power-saving to default mode if there is Wi-Fi connectionless module coexists with non-Wi-Fi module.
