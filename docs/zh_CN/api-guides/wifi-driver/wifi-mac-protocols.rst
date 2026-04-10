Wi-Fi MAC 协议
==========================

:link_to_translation:`en:[English]`

Wi-Fi HT20/40
-------------------------

.. only:: esp32 or esp32s2 or esp32c3 or esp32s3 or esp32c6 or esp32c5

    {IDF_TARGET_NAME} 支持 Wi-Fi 带宽 HT20 或 HT40，不支持 HT20/40 共存，调用函数 :cpp:func:`esp_wifi_set_bandwidth()` 可改变 station/AP 的默认带宽。{IDF_TARGET_NAME} station 和 AP 的默认带宽为 HT40。

    station 模式下，实际带宽首先在 Wi-Fi 连接时协商。只有当 station 和所连 AP 都支持 HT40 时，带宽才为 HT40，否则为 HT20。如果所连的 AP 的带宽发生变化，则在不断开 Wi-Fi 连接的情况下再次协商实际带宽。

    同样，在 AP 模式下，在 AP 与所连 station 协商实际带宽。如果 AP 和其中一个 station 支持 HT40， 则为 HT40， 否则为 HT20。

    在 station/AP 共存模式下，station 和 AP 都可独立配置为 HT20/40。如果 station 和 AP 都协商为 HT40，由于 {IDF_TARGET_NAME} 中，station 的优先级总高于 AP，HT40 信道是 station 的信道。例如，AP 的配置带宽为 HT40，配置的主信道为 6，配置的辅助信道为 10。如果，station 所连路由器的主信道为 6、辅助信道为 2，AP 的实际信道将自动更改为主 6 和辅 2。

    理论上，HT40 可以获得更大的吞吐量，因为 HT40 的最大原始 PHY 数据速率为 150 Mbps，而 HT20 为 72 Mbps。但是，如果设备在某些特殊环境中使用，例如，{IDF_TARGET_NAME} 周围其他 Wi-Fi 设备过多，HT40 的性能可能会降低。因此，如果应用程序需要支持相同或类似的情况，建议始终将带宽配置为 HT20。

.. only:: esp32c5

    .. note::

        当工作在 2.4 GHz + 5 GHz 频段模式(``WIFI_BAND_MODE_AUTO``)时，需要使用函数: :cpp:func:`esp_wifi_set_bandwidths()` 分别设置 2.4 GHz 频段 和 5 GHz 频段的带宽。


.. only:: esp32c2

    {IDF_TARGET_NAME} 仅支持 Wi-Fi 带宽 HT20，不支持 Wi-Fi 带宽 HT40 或 HT20/40 共存。

Wi-Fi QoS
-------------------------

{IDF_TARGET_NAME} 支持 WFA Wi-Fi QoS 认证所要求的所有必备功能。

Wi-Fi 协议中定义了四个 AC（访问类别），每个 AC 有各自的优先级访问 Wi-Fi 信道。此外，还定义了映射规则以映射其他协议的 QoS 优先级，例如 802.11D 或 TCP/IP 到 Wi-Fi AC。

下表描述 {IDF_TARGET_NAME} 中 IP 优先级如何映射到 Wi-Fi AC，还指明此 AC 是否支持 AMPDU。该表按优先级降序排列，即 AC_VO 拥有最高优先级。

+-----------+---------------------+----------------+
| IP 优先级 | Wi-Fi AC            | 是否支持 AMPDU |
+===========+=====================+================+
| 6, 7      | AC_VO (Voice)       | 否             |
+-----------+---------------------+----------------+
| 4, 5      | AC_VI (Video)       | 是             |
+-----------+---------------------+----------------+
| 3, 0      | AC_BE (Best Effort) | 是             |
+-----------+---------------------+----------------+
| 1, 2      | AC_BK (Background)  | 是             |
+-----------+---------------------+----------------+

应用程序可以通过套接字选项 IP_TOS 配置 IP 优先级使用 QoS 功能。下面是使套接字使用 VI 队列的示例::

    const int ip_precedence_vi = 4;
    const int ip_precedence_offset = 5;
    int priority = (ip_precedence_vi << ip_precedence_offset);
    setsockopt(socket_id, IPPROTO_IP, IP_TOS, &priority, sizeof(priority));

理论上，高优先级的 AC 比低优先级 AC 具有更好的性能，但并非总是如此，下面是一些关于如何使用 Wi-Fi QoS 的建议：

 - 可以把一些真正重要的应用程序流量放到 AC_VO 队列中。避免通过 AC_VO 队列发送大流量。一方面，AC_VO 队列不支持 AMPDU，如果流量很大，性能不会优于其他队列。另一方面，可能会影响同样使用 AC_VO 队列的管理帧。
 - 避免使用 AMPDU 支持的、两个以上的不同优先级，比如 socket A 使用优先级 0，socket B 使用优先级 1，socket C 使用优先级 2。因为可能需要更多的内存，不是好的设计。具体来说，Wi-Fi 驱动程序可能会为每个优先级生成一个 Block Ack 会话，如果设置了 Block Ack 会话，则需要更多内存。


Wi-Fi AMPDU
--------------------

{IDF_TARGET_NAME} 同时支持接收和发送 AMPDU，AMPDU 可以大大提高 Wi-Fi 的吞吐量。

通常，应使能 AMPDU。禁用 AMPDU 通常用于调试目的。


Wi-Fi AMSDU
-------------------------

.. only:: not SOC_SPIRAM_SUPPORTED

    {IDF_TARGET_NAME} 支持接收 AMSDU。

.. only:: SOC_SPIRAM_SUPPORTED

    {IDF_TARGET_NAME} 支持接收和发送 AMSDU。开启 AMSDU 发送比较消耗内存，默认不开启 AMSDU 发送。可通过选项 :ref:`CONFIG_ESP_WIFI_AMSDU_TX_ENABLED` 使能 AMSDU 发送功能， 但是使能 AMSDU 发送依赖于 :ref:`CONFIG_SPIRAM` 。

Wi-Fi 分片
-------------------------

.. only:: not SOC_WIFI_TXOP_SUPPORT

    {IDF_TARGET_NAME} 支持 Wi-Fi 接收分片，但不支持 Wi-Fi 发送分片。

.. only:: SOC_WIFI_TXOP_SUPPORT

    {IDF_TARGET_NAME} 支持 Wi-Fi 接收和发送分片。
