安全与漫游
=================

:link_to_translation:`en:[English]`


支持的 Wi-Fi 安全模式
-------------------------

{IDF_TARGET_NAME} 支持以下 Wi-Fi 安全模式。有关详细配置和使用说明，请参阅 :doc:`Wi-Fi 安全性 <../wifi-security>`。

- Open
- WEP
- 个人级 WPA
- 个人级 WPA2
- 个人级 WPA3
- 企业级 Wi-Fi


Wi-Fi Easy Connect™ (DPP)
--------------------------

Wi-Fi Easy Connect\ :sup:`TM` （也称为设备配置协议）是一个安全且标准化的配置协议，用于配置 Wi-Fi 设备。更多信息请参考 :doc:`esp_dpp <../../api-reference/network/esp_dpp>`。


WPS 注册
-------------------------

在 Wi-Fi 模式 WIFI_MODE_STA 或 WIFI_MODE_APSTA 下，{IDF_TARGET_NAME} 支持 WPS 注册功能。目前，{IDF_TARGET_NAME} 支持的 WPS enrollee 类型有 PBC 和 PIN。


无线网络管理
----------------------------

无线网络管理让客户端设备能够交换有关网络拓扑结构的信息，包括与射频环境相关的信息。这使每个客户端都能感知网络状况，从而促进无线网络性能的整体改进。这是 802.11v 规范的一部分。它还使客户端能够支持网络辅助漫游。
网络辅助漫游让 WLAN 能够向关联的客户端发送消息，从而使客户端与具有更好链路指标的 AP 关联。这对于促进负载平衡以及引导连接不良的客户端都很有用。

目前 802.11v 的实现支持 BSS 过渡管理帧。

无线资源管理
---------------------------

无线电资源测量 (802.11k) 旨在改善网络内流量的分配方式。在无线局域网中，一般情况下，无线设备会连接发射信号最强的接入点 (AP)。根据用户的数量和地理位置，这种分配方式有时会导致某个接入点超负荷而其它接入点利用不足，从而导致整体网络性能下降。在符合 802.11k 规范的网络中，如果信号最强的 AP 已满负荷，无线设备则转移到其它未充分利用的 AP。尽管信号可能较弱，但由于更有效地利用了网络资源，总体吞吐量会更大。

目前 802.11k 的实现支持信标测量报告、链路测量报告和邻居请求。

请参考 IDF 示例程序 :idf_file:`examples/wifi/roaming/README.md` 来设置和使用这些 API。示例代码只演示了如何使用这些 API，应用程序应根据需要定义自己的算法和案例。

快速 BSS 切换
---------------

快速 BSS 切换 (802.11r FT) 是一种标准，旨在允许移动中的无线设备保持连续连接，让客户端从一个 BSS（Basic Service Set，基本服务集，口头也称 AP）快速安全地切换到另一个 BSS， 实现几乎无缝的连接， **避免 802.11i 的四次握手过程**。802.11r 重新定义了安全密钥协商协议，规定了 AP 之间的转换方式，允许以并行的方式协商和请求无线资源。服务器生成的密钥会缓存在无线网络中，未来可以基于此缓存的密钥建立一定数量的连接，避免了 802.11x 过程。


{IDF_TARGET_NAME} station 模式支持 WPA2-PSK 网络 FT。注意，{IDF_TARGET_NAME} station 模式仅支持空中 FT 协议。

在 :cpp:type:`wifi_sta_config_t` 中提供了配置选项 :ref:`CONFIG_ESP_WIFI_11R_SUPPORT` 和配置参数 :cpp:type:`ft_enabled`，可启用 station 的 802.11r 支持。详细信息请参阅 ESP-IDF 示例 :idf_file:`examples/wifi/roaming/README.md`。

.. only:: SOC_WIFI_FTM_SUPPORT

    Wi-Fi Location
    -------------------------------

    Wi-Fi Location 将提高 AP 以外设备位置数据的准确性，这有助于创建新的、功能丰富的应用程序和服务，例如地理围栏、网络管理、导航等。用于确定设备相对于接入点的位置的协议之一是精细定时测量 (FTM)，它会计算 Wi-Fi 帧的飞行时间。

    精细定时测量 (FTM)
    +++++++++++++++++++++++++++++

    FTM 用于测量 Wi-Fi 往返时间（Wi-Fi RTT)，即 Wi-Fi 信号从一个设备到另一个设备并返回所需的时间。使用 Wi-Fi RTT，设备之间的距离可以用一个简单的公式 `RTT * c / 2` 来计算，其中 c 是光速。

    对于设备之间交换的帧，FTM 在帧到达或离开时使用时间戳，这个时间戳由 Wi-Fi 接口硬件提供。FTM 发起方（主要是 station 设备）发现 FTM 响应方（可以是 station 或 AP），并协商启动 FTM 程序。该程序以突发形式发送的多个动作帧及其 ACK 来收集时间戳数据。FTM 发起方最后收集数据以计算平均往返时间。

    {IDF_TARGET_NAME} 在以下配置中支持 FTM：

    - {IDF_TARGET_NAME} 在 station 模式下为 FTM 发起方。
    - {IDF_TARGET_NAME} 在 AP 模式下为 FTM 响应方。

.. only:: esp32c6

     {IDF_TARGET_NAME} ECO1 及更早的版本不支持 FTM 发起方模式。

.. attention::

     使用 RTT 的距离测量并不准确，RF 干扰、多径传播、天线方向和缺乏校准等因素会增加这些不准确度。为了获得更好的结果，建议在两个 ESP32 芯片系列设备（ESP32-C2 除外）之间执行 FTM，这两个设备可分别设置为 station 和 AP 模式。

     请参考 ESP-IDF 示例 :idf_file:`examples/wifi/ftm/README.md`，了解设置和执行 FTM 的详细步骤。
