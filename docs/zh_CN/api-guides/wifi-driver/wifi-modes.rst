Wi-Fi 模式
===============

:link_to_translation:`en:[English]`

.. _wifi-ap-general-scenario:

{IDF_TARGET_NAME} Wi-Fi AP 通用场景
---------------------------------------------

下图为 AP 模式下的宏观场景，其中包含不同阶段的具体描述：

 .. seqdiag::
    :caption: AP 模式下 Wi-Fi 事件场景示例
    :align: center

    seqdiag sample-scenarios-soft-ap-mode {
        activation = none;
        node_width = 80;
        node_height = 60;
        edge_length = 140;
        span_height = 5;
        default_shape = roundedbox;
        default_fontsize = 12;

        MAIN_TASK  [label = "Main\ntask"];
        APP_TASK   [label = "App\ntask"];
        EVENT_TASK [label = "Event\ntask"];
        LwIP_TASK  [label = "LwIP\ntask"];
        WIFI_TASK  [label = "Wi-Fi\ntask"];

        === 1. 初始化阶段 ===
        MAIN_TASK  ->  LwIP_TASK   [label="1.1> 创建/初始化 LwIP"];
        MAIN_TASK  ->  EVENT_TASK  [label="1.2> 创建/初始化事件"];
        MAIN_TASK  ->  WIFI_TASK   [label="1.3> 创建/初始化 Wi-Fi"];
        MAIN_TASK  ->  APP_TASK    [label="1.4> 创建应用程序任务"];
        === 2. 配置阶段 ===
        MAIN_TASK  ->  WIFI_TASK   [label="2> 配置 Wi-Fi"];
        === 3. 启动阶段 ===
        MAIN_TASK  ->  WIFI_TASK   [label="3.1> 启动 Wi-Fi"];
        EVENT_TASK <-  WIFI_TASK   [label="3.2> WIFI_EVENT_AP_START"];
        APP_TASK   <-  EVENT_TASK  [label="3.3> WIFI_EVENT_AP_START"];
        === 4. 连接阶段 ===
        EVENT_TASK <-  WIFI_TASK   [label="4.1> WIFI_EVENT_AP_STACONNECTED"];
        APP_TASK   <- EVENT_TASK   [label="4.2> WIFI_EVENT_AP_STACONNECTED"];
        === 5. 断开阶段 ===
        EVENT_TASK <-  WIFI_TASK   [label="5.1> WIFI_EVENT_AP_STADISCONNECTED"];
        APP_TASK   <-  EVENT_TASK  [label="5.2> WIFI_EVENT_AP_STADISCONNECTED"];
        APP_TASK   ->  APP_TASK    [label="5.3> 断开处理"];
        === 6. 清理阶段 ===
        APP_TASK   ->  WIFI_TASK   [label="6.1> 断开 Wi-Fi 连接"];
        APP_TASK   ->  WIFI_TASK   [label="6.2> 终止 Wi-Fi"];
        APP_TASK   ->  WIFI_TASK   [label="6.3> 清理 Wi-Fi"];
    }


Wi-Fi Sniffer 模式
---------------------------

Wi-Fi Sniffer 模式可以通过 :cpp:func:`esp_wifi_set_promiscuous()` 使能。如果使能 Sniffer 模式， **可以** 向应用程序转储以下数据包。

 - 802.11 管理帧
 - 802.11 数据帧，包括 MPDU、AMPDU、AMSDU 等
 - 802.11 MIMO 帧，Sniffer 模式仅转储 MIMO 帧的长度。
 - 802.11 控制帧
 - 802.11 CRC 错误帧

**不可以** 向应用程序转储以下数据包。

 - 802.11 其它错误帧

对于 Sniffer 模式 **可以** 转储的帧，应用程序可以另外使用 :cpp:func:`esp_wifi_set_promiscuous_filter()` 和 :cpp:func:`esp_wifi_set_promiscuous_ctrl_filter()` 决定筛选哪些特定类型的数据包。应用程序默认筛选所有 802.11 数据和管理帧。如果你想要筛选 802.11 控制帧，:cpp:func:`esp_wifi_set_promiscuous_filter()` 中的 filter 参数需要包含 “WIFI_PROMIS_FILTER_MASK_CTRL” 类型， 如果你想进一步区分 802.11 控制帧，那么调用 :cpp:func:`esp_wifi_set_promiscuous_ctrl_filter()`。

可以在 WIFI_MODE_NULL、WIFI_MODE_STA、WIFI_MODE_AP、WIFI_MODE_APSTA 等 Wi-Fi 模式下使能 Wi-Fi Sniffer 模式。也就是说，当 station 连接到 AP，或者 AP 有 Wi-Fi 连接时，就可以使能。请注意，Sniffer 模式对 station/AP Wi-Fi 连接的吞吐量有 **很大影响**。通常，除非有特别原因，当 station/AP Wi-Fi 连接出现大量流量，不应使能。

该模式下还应注意回调函数 :cpp:type:`wifi_promiscuous_cb_t` 的使用。该回调将直接在 Wi-Fi 驱动程序任务中进行，所以如果应用程序需处理大量过滤的数据包，建议在回调中向应用程序任务发布一个事件，把真正的工作推迟到应用程序任务中完成。


.. only:: SOC_WIFI_NAN_SUPPORT

    Wi-Fi Aware\ :sup:`TM` (NAN)
    ----------------------------

    Wi-Fi Aware\ :sup:`TM` 或 NAN，即 Neighbor Awareness Networking (邻居感知网络，下文简称 NAN) 协议可以让 Wi-Fi 设备发现附近的其他设备。NAN 使用设备间直接通信，无需通过互联网或 AP 连接。

    多个邻近的 NAN 设备组成一个 NAN 集群，集群中的设备可相互通信。同一集群中的 NAN 设备会同步时钟，并定期在信道 6 上互相监听。NAN 设备还可通过服务发现协议，在所处集群内提供（发布）或获取（订阅）服务。通过服务名称和匹配过滤器（可选），可以完成服务匹配。当订阅设备匹配到发布设备时，就可以发送信息（跟进）或与发布设备建立数据通路 (NDP)。NDP 建立后，订阅设备和发布设备都会获得一个 IPv6 地址用于通信。

    注意，数据包发送是不加密的，无法保证 NDP 的安全性。NAN 为发现 (Discovery) 和数据通路单独的接口，该接口与 STA 和 AP 所用的接口不同。NAN 以独立模式运行，不支持与 STA 或 AP 接口共存。

    要了解如何设置 NAN 发布设备和订阅设备，请参考 ESP-IDF 示例：:idf_file:`examples/wifi/wifi_aware/nan_publisher/README.md` 和 :idf_file:`examples/wifi/wifi_aware/nan_subscriber/README.md`。


    非同步服务发现 (USD)
    ---------------------------

    非同步服务发现 (USD) 是一种机制，可让设备在新的设备进入射频环境时发现其提供的可用服务，而无需设备之间进行同步。

    USD 通过发布 (Publish) 及跟进消息 (Follow-up message) 中的 SDEA 的服务信息 (Service Info) 字段来传递服务相关信息。

    要了解如何设置 NAN-USD 发布设备和订阅设备，请参考 ESP-IDF 示例：:idf_file:`examples/wifi/wifi_aware/usd_publisher/README.md` 和 :idf_file:`examples/wifi/wifi_aware/usd_subscriber/README.md`。
