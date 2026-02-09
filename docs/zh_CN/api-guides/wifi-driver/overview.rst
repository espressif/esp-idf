概览
=====

:link_to_translation:`en:[English]`

{IDF_TARGET_NAME} Wi-Fi 功能列表
------------------------------------

{IDF_TARGET_NAME} 支持以下 Wi-Fi 功能：

.. only:: esp32 or esp32s2 or esp32c3 or esp32s3

    - 支持 4 个虚拟 Wi-Fi 接口，即 STA、AP、Sniffer 和保留接口
    - 支持仅 station 模式、仅 AP 模式、station/AP 共存模式
    - 支持使用 IEEE 802.11b、IEEE 802.11g、IEEE 802.11n 和提供配置协议模式的 API
    - 支持 WPA/WPA2/WPA3/WPA2-企业版/WPA3-企业版/WAPI/WPS 和 DPP
    - 支持 AMSDU、AMPDU、HT40、QoS 以及其它关键功能
    - 支持 Modem-sleep
    - 支持乐鑫专属 ESP-NOW 协议和长距离模式，可实现最远 **1 km** 的数据传输
    - 空中数据传输最高可达 20 MBit/s TCP 吞吐量和 30 MBit/s UDP 吞吐量
    - 支持 Sniffer
    - 支持快速扫描和全信道扫描
    - 支持多天线
    - 支持获取信道状态信息


.. only:: esp32c2

    - 支持 3 个虚拟接口，即 STA、AP 和 Sniffer
    - 支持仅 station 模式、仅 AP 模式、station/AP 共存模式
    - 支持使用 IEEE 802.11b、IEEE 802.11g、IEEE 802.11n 和 API 配置协议模式
    - 支持 WPA/WPA2/WPA3/WPA2-企业版/WPA3-企业版/WPS 和 DPP
    - 支持 AMPDU、QoS 以及其它主要功能
    - 支持 Modem-sleep
    - 空中数据传输最高可达 20 MBit/s TCP 吞吐量和 30 MBit/s UDP 吞吐量
    - 支持 Sniffer
    - 支持快速扫描和全信道扫描
    - 支持多个天线


.. only:: esp32c6

    - 支持 4 个虚拟接口，即 STA、AP、Sniffer 和 reserved
    - 支持仅 station 模式、仅 AP 模式、station/AP 共存模式
    - 支持使用 IEEE 802.11b、IEEE 802.11g、IEEE 802.11n、IEEE 802.11ax 和 API 配置协议模式
    - 支持 WPA/WPA2/WPA3/WPA2-企业版/WPA3-企业版/WAPI/WPS 和 DPP
    - 支持 AMSDU、AMPDU、HT40、QoS 以及其它主要功能
    - 支持 Modem-sleep
    - 支持乐鑫专属协议，可实现 **1 km** 数据通信量
    - 空中数据传输最高可达 20 MBit/s TCP 吞吐量和 30 MBit/s UDP 吞吐量
    - 支持 Sniffer
    - 支持快速扫描和全信道扫描
    - 支持多个天线
    - 支持获取信道状态信息
    - 支持 Individual TWT 和 Broadcast TWT
    - 支持下行 MU-MIMO
    - 支持 OFDMA
    - 支持 BSS Color


.. only:: esp32c5

    - 支持 4 个虚拟接口，即 STA、AP、Sniffer 和 reserved
    - 支持仅 station 模式、仅 AP 模式、station/AP 共存模式
    - 支持使用 IEEE 802.11b、IEEE 802.11g、IEEE 802.11n、IEEE 802.11a、IEEE 802.11ac、IEEE 802.11ax 和 API 配置协议模式
    - 支持 WPA/WPA2/WPA3/WPA2-企业版/WPA3-企业版/WAPI/WPS 和 DPP
    - 支持 AMSDU、AMPDU、HT40、QoS 以及其它主要功能
    - 支持 Modem-sleep
    - 2.4 GHz 支持乐鑫专属协议，可实现 **1 km** 数据通信量
    - 空中数据传输最高可达 20 MBit/s TCP 吞吐量和 30 MBit/s UDP 吞吐量
    - 支持 Sniffer
    - 支持快速扫描和全信道扫描
    - 支持多个天线
    - 支持获取信道状态信息
    - 支持 Individual TWT 和 Broadcast TWT
    - 支持下行 MU-MIMO
    - 支持 OFDMA
    - 支持 BSS Color
.. only:: SOC_WIFI_NAN_SUPPORT

    - Wi-Fi Aware (NAN)


如何编写 Wi-Fi 应用程序
----------------------------------

准备工作
+++++++++++

一般来说，要编写自己的 Wi-Fi 应用程序，最高效的方式是先选择一个相似的应用程序示例，然后将其中可用的部分移植到自己的项目中。如果你希望编写一个强健的 Wi-Fi 应用程序，强烈建议在开始之前先阅读本文。**非强制要求，请依个人情况而定。**

本文将补充说明 Wi-Fi API 和 Wi-Fi 示例的相关信息，重点描述使用 Wi-Fi API 的原则、当前 Wi-Fi API 实现的限制以及使用 Wi-Fi 时的常见错误。同时，本文还介绍了 Wi-Fi 驱动程序的一些设计细节。建议选择一个示例 :example:`example <wifi>` 进行参考。

- :example:`wifi/getting_started/station` 演示如何使用 station 功能连接到 AP。

- :example:`wifi/getting_started/softAP` 演示如何使用 SoftAP 功能将 {IDF_TARGET_NAME} 配置为 AP。

- :example:`wifi/scan` 演示如何扫描可用的 AP，配置扫描设置，并显示扫描结果。

- :example:`wifi/fast_scan` 演示如何执行快速和全信道扫描，查找附近的 AP，设置信号强度的阈值和认证模式，并根据信号强度和认证模式连接到最合适的 AP。

- :example:`wifi/wps` 演示如何使用 WPS 入网功能，简化连接 Wi-Fi 路由器的过程，支持 PIN 或 PBC 模式。

- :example:`wifi/wps_softap_registrar` 演示如何在 SoftAP 模式下使用 WPS 注册器功能，从而简化从 station 连接到 Wi-Fi SoftAP 的过程。

- :example:`wifi/smart_config` 演示如何使用 smartconfig 功能通过 ESPTOUCH app 连接到目标 AP。

- :example:`wifi/power_save` 演示如何使用 station 模式的省电模式。

- :example:`wifi/softap_sta` 演示如何配置 {IDF_TARGET_NAME} 同时用作 AP 和 station，从而可将其用作 Wi-Fi NAT 路由器。

- :example:`wifi/iperf` 演示如何实现 iPerf 性能测量工具所使用的协议，允许在两个芯片之间或在单个芯片和运行 iPerf 工具的计算机之间进行性能测量，并提供测试 station/SoftAP TCP/UDP RX/TX 吞吐量的具体说明。

- :example:`wifi/roaming/roaming_app` 演示如何使用 Wi-Fi Roaming App 功能，在兼容的 AP 之间高效漫游。

- :example:`wifi/roaming/roaming_11kvr` 演示如何使用 11k 和 11v API 实现漫游功能。

.. only:: SOC_WIFI_HE_SUPPORT

    - :example:`wifi/itwt` 演示如何使用 iTWT 功能，该功能仅在 station 模式下工作，并在不同的省电模式下提供设置、拆卸和挂起的命令，还展示了启用和禁用 iTWT 时的电流消耗差异。

设置 Wi-Fi 编译时选项
+++++++++++++++++++++++

请参阅 :ref:`wifi-menuconfig`。

Wi-Fi 初始化
+++++++++++++++

请参阅 :ref:`{IDF_TARGET_NAME} Wi-Fi station 通用场景 <wifi-station-general-scenario>`、:ref:`{IDF_TARGET_NAME} Wi-Fi AP 通用场景 <wifi-ap-general-scenario>`。

启动/连接 Wi-Fi
++++++++++++++++++

请参阅 :ref:`{IDF_TARGET_NAME} Wi-Fi station 通用场景 <wifi-station-general-scenario>`、:ref:`{IDF_TARGET_NAME} Wi-Fi AP 通用场景 <wifi-ap-general-scenario>`。

事件处理
++++++++++++++

通常，在理想环境下编写代码难度并不大，如 :ref:`WIFI_EVENT_STA_START <wifi-event-sta-start>`、:ref:`WIFI_EVENT_STA_CONNECTED <wifi-event-sta-connected>` 中所述。难度在于如何在现实的困难环境下编写代码，如 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 中所述。能否在后者情况下完美地解决各类事件冲突，是编写一个强健的 Wi-Fi 应用程序的根本。请参阅 :ref:`{IDF_TARGET_NAME} Wi-Fi 事件描述 <wifi-event-description>`, :ref:`{IDF_TARGET_NAME} Wi-Fi station 通用场景 <wifi-station-general-scenario>`, :ref:`{IDF_TARGET_NAME} Wi-Fi AP 通用场景 <wifi-ap-general-scenario>`。另可参阅 ESP-IDF 中的 :doc:`事件处理概述 <../../api-reference/system/esp_event>`。

编写错误恢复程序
++++++++++++++++++++++++++++++++++++++++++++++++++++

除了能在比较差的环境下工作，错误恢复能力也对一个强健的 Wi-Fi 应用程序至关重要。请参阅 :ref:`{IDF_TARGET_NAME} Wi-Fi API 错误代码 <wifi-api-error-code>`。


.. _wifi-api-error-code:

{IDF_TARGET_NAME} Wi-Fi API 错误代码
--------------------------------------

所有 {IDF_TARGET_NAME} Wi-Fi API 都有定义好的返回值，即错误代码。这些错误代码可分类为：

 - 无错误，例如：返回值 :c:macro:`ESP_OK` 代表 API 成功返回
 - 可恢复错误，例如：:c:macro:`ESP_ERR_NO_MEM`
 - 不可恢复的非关键性错误
 - 不可恢复的关键性错误

一个错误是否为关键性取决于其 API 和应用场景，并且由 API 用户定义。

**要使用 Wi-Fi API 编写一个强健的应用程序，根本原则便是要时刻检查错误代码并编写相应的错误处理代码。** 一般来说，错误处理代码可用于解决：

 - 可恢复错误，你可以编写相应的错误恢复代码。例如，当 :cpp:func:`esp_wifi_start()` 返回 :c:macro:`ESP_ERR_NO_MEM` 时，调用可恢复错误处理代码 vTaskDelay 以获取微秒级的延迟后重新尝试。
 - 不可恢复非关键性错误，打印错误代码可以帮助你更好地处理该类错误。
 - 不可恢复关键性错误，可使用 "assert" 语句处理该类错误。例如，如果 :cpp:func:`esp_wifi_set_mode()` 返回 ``ESP_ERR_WIFI_NOT_INIT``，这意味着 Wi-Fi 驱动程序未通过 :cpp:func:`esp_wifi_init()` 成功初始化。这类错误可以在应用开发阶段被快速检测到。

在 :component_file:`esp_common/include/esp_err.h` 中， ``ESP_ERROR_CHECK`` 负责检查返回值。这是一个较为常见的错误处理代码，可在应用程序开发阶段作为默认的错误处理代码。但是，我们强烈建议 API 的使用者编写自己的错误处理代码。


初始化 {IDF_TARGET_NAME} Wi-Fi API 参数
----------------------------------------------------

在为 API 初始化结构体参数时，应遵循以下两种方式之一：

- 显式设置参数的所有字段
- 先使用 get API 获取当前配置，然后设置特定于应用程序的字段

初始化或获取整个结构体这一步至关重要，因为大多数情况下，值为 0 意味着程序使用了默认值。未来可能会向该结构体添加更多字段，而将这些字段初始化为 0 可以确保在 ESP-IDF 更新到新版本后，应用程序仍能正常运行。


.. _wifi-programming-model:

{IDF_TARGET_NAME} Wi-Fi 编程模型
-----------------------------------------

{IDF_TARGET_NAME} Wi-Fi 编程模型如下图所示：

.. blockdiag::
    :caption: Wi-Fi 编程模型
    :align: center

    blockdiag wifi-programming-model {

        # global attributes
        node_height = 60;
        node_width = 100;
        span_width = 100;
        span_height = 60;
        default_shape = roundedbox;
        default_group_color = none;

        # node labels
        TCP_STACK [label="TCP\n stack", fontsize=12];
        EVNT_TASK [label="Event\n task", fontsize=12];
        APPL_TASK [label="Application\n task", width = 120, fontsize=12];
        WIFI_DRV  [label="Wi-Fi\n Driver", width = 120, fontsize=12];
        KNOT [shape=none];

        # node connections + labels
        TCP_STACK -> EVNT_TASK [label=event];
        EVNT_TASK -> APPL_TASK [label="callback\n or event"];

        # arrange nodes vertically
        group {
           label = "default handler";
           orientation = portrait;
           EVNT_TASK <- WIFI_DRV [label=event];
        }

        # intermediate node
        group {
            label = "user handler";
            orientation = portrait;
            APPL_TASK -- KNOT;
        }
        WIFI_DRV <- KNOT [label="API\n call"];
    }


Wi-Fi 驱动程序可以看作是一个无法感知上层代码（如 TCP/IP 堆栈、应用程序任务、事件任务等）的黑匣子。通常，应用程序任务（代码）负责调用 :doc:`Wi-Fi 驱动程序 API <../../api-reference/network/esp_wifi>` 来初始化 Wi-Fi，并在必要时处理 Wi-Fi 事件。然后，Wi-Fi 驱动程序接收 API 调用，处理后将事件发送给应用程序。

Wi-Fi 事件处理是在 :doc:`esp_event 库 <../../api-reference/system/esp_event>` 的基础上进行的。Wi-Fi 驱动程序将事件发送至 :ref:`默认事件循环 <esp-event-default-loops>`，应用程序便可以使用 :cpp:func:`esp_event_handler_register()` 中的回调函数处理这些事件。除此之外，:doc:`esp_netif 组件 <../../api-reference/network/esp_netif>` 也负责处理 Wi-Fi 事件，并产生一系列默认行为。例如，当 Wi-Fi station 连接至一个 AP 时，esp_netif 将自动开启 DHCP 客户端服务（系统默认）。


.. _wifi-event-description:

{IDF_TARGET_NAME} Wi-Fi 事件描述
-----------------------------------------

WIFI_EVENT_WIFI_READY
++++++++++++++++++++++++++++++++++++

Wi-Fi 驱动程序永远不会生成此事件，因此，应用程序的事件回调函数可忽略此事件。在未来的版本中，此事件可能会被移除。

.. _wifi-event-scan-done:

WIFI_EVENT_SCAN_DONE
++++++++++++++++++++++++++++++++++++

扫描完成事件，由 :cpp:func:`esp_wifi_scan_start()` 函数触发，将在以下情况下产生：

  - 扫描已完成，例如：Wi-Fi 已成功找到目标 AP 或已扫描所有信道。
  - 当前扫描因函数 :cpp:func:`esp_wifi_scan_stop()` 而终止。
  - 在当前扫描完成之前调用了函数 :cpp:func:`esp_wifi_scan_start()`。此时，新的扫描将覆盖当前扫描过程，并生成一个扫描完成事件。

以下情况下将不会产生扫描完成事件：

  - 当前扫描为阻塞扫描。
  - 当前扫描是由函数 :cpp:func:`esp_wifi_connect()` 触发的。

接收到此事件后，事件任务暂不做任何响应。首先，应用程序的事件回调函数需调用 :cpp:func:`esp_wifi_scan_get_ap_num()` 和 :cpp:func:`esp_wifi_scan_get_ap_records()` 获取已扫描的 AP 列表，然后触发 Wi-Fi 驱动程序释放在扫描过程中占用的内存空间（**切记该步骤**）。
更多详细信息，请参阅 :ref:`{IDF_TARGET_NAME} Wi-Fi 扫描 <wifi-scan>`。

.. _wifi-event-sta-start:

WIFI_EVENT_STA_START
++++++++++++++++++++++++++++++++++++

如果调用函数 :cpp:func:`esp_wifi_start()` 后接收到返回值 :c:macro:`ESP_OK`，且当前 Wi-Fi 处于 station 或 station/AP 共存模式，则将产生此事件。接收到此事件后，事件任务将初始化 LwIP 网络接口 (netif)。通常，应用程序的事件回调函数需调用 :cpp:func:`esp_wifi_connect()` 来连接已配置的 AP。

WIFI_EVENT_STA_STOP
++++++++++++++++++++++++++++++++++++

如果调用函数 :cpp:func:`esp_wifi_stop()` 后接收到返回值 :c:macro:`ESP_OK`，且当前 Wi-Fi 处于 station 或 station/AP 共存模式，则将产生此事件。接收到此事件后，事件任务将进行释放 station IP 地址、终止 DHCP 客户端服务、移除 TCP/UDP 相关连接并清除 LwIP station netif 等动作。此时，应用程序的事件回调函数通常不需做任何响应。

.. _wifi-event-sta-connected:

WIFI_EVENT_STA_CONNECTED
++++++++++++++++++++++++++++++++++++

如果调用函数 :cpp:func:`esp_wifi_connect()` 后接收到返回值 :c:macro:`ESP_OK`，且 station 已成功连接目标 AP，则将产生此连接事件。接收到此事件后，事件任务将启动 DHCP 客户端服务并开始获取 IP 地址。此时，Wi-Fi 驱动程序已准备就绪，可发送和接收数据。如果你的应用程序不依赖于 LwIP（即 IP 地址），则此刻便可以开始应用程序开发工作。但是，如果你的应用程序需基于 LwIP 进行，则还需等待 *got ip* 事件发生后才可开始。

.. _wifi-event-sta-disconnected:

WIFI_EVENT_STA_DISCONNECTED
++++++++++++++++++++++++++++++++++++

此事件将在以下情况下产生：

  - 调用了函数 :cpp:func:`esp_wifi_disconnect()` 或 :cpp:func:`esp_wifi_stop()`，且 Wi-Fi station 已成功连接至 AP。
  - 调用了函数 :cpp:func:`esp_wifi_connect()`，但 Wi-Fi 驱动程序因为某些原因未能成功连接至 AP，例如：未扫描到目标 AP、验证超时等。或存在多个 SSID 相同的 AP，station 无法连接所有已找到的 AP，也将产生该事件。
  - Wi-Fi 连接因为某些原因而中断，例如：station 连续多次丢失 N beacon、AP 踢掉 station、AP 认证模式改变等。

接收到此事件后，事件任务的默认动作为：

  - 关闭 station 的 LwIP netif。
  - 通知 LwIP 任务清除导致所有套接字状态错误的 UDP/TCP 连接。针对基于套接字编写的应用程序，其回调函数可以在接收到此事件时（如有必要）关闭并重新创建所有套接字。

应用程序处理此事件最常用的方法为：调用函数 :cpp:func:`esp_wifi_connect()` 重新连接 Wi-Fi。但是，如果此事件是由函数 :cpp:func:`esp_wifi_disconnect()` 引发的，则应用程序不应调用 :cpp:func:`esp_wifi_connect()` 来重新连接。应用程序须明确区分此事件的引发原因，因为某些情况下应使用其它更好的方式进行重新连接。请参阅 :ref:`Wi-Fi 重新连接 <wifi-reconnect>` 和 :ref:`连接 Wi-Fi 时扫描 <scan-when-wifi-is-connecting>`。

需要注意的另一点是：接收到此事件后，LwIP 的默认动作是终止所有 TCP 套接字连接。大多数情况下，该动作不会造成影响。但对某些特殊应用程序可能除外。例如：

- 应用程序创建一个了 TCP 连接，以维护每 60 秒发送一次的应用程序级、保持活动状态的数据。
- 由于某些原因，Wi-Fi 连接被切断并引发了 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件。根据当前实现，此时所有 TCP 连接都将被移除，且保持活动的套接字将处于错误的状态中。但是，由于应用程序设计者认为网络层 **不应** 考虑这个 Wi-Fi 层的错误，因此应用程序不会关闭套接字。
- 5 秒后，因为在应用程序的事件回调函数中调用了 :cpp:func:`esp_wifi_connect()`，Wi-Fi 连接恢复。**同时，station 连接至同一个 AP 并获得与之前相同的 IPV4 地址。**
- 60 秒后，当应用程序发送具有保持活动状态的套接字的数据时，套接字将返回错误，应用程序将关闭套接字并在必要时重新创建。

在上述场景中，理想状态下应用程序套接字和网络层将不会受到影响，因为在此过程中 Wi-Fi 连接只是短暂地断开然后快速恢复。

.. _ip-event-sta-got-ip:

IP_EVENT_STA_GOT_IP
++++++++++++++++++++++++++++++++++++

当 DHCP 客户端成功从 DHCP 服务器获取 IPV4 地址或 IPV4 地址发生改变时，将引发此事件。此事件意味着应用程序一切就绪，可以开始任务（如：创建套接字）。

IPV4 地址可能由于以下原因而发生改变：

  - DHCP 客户端无法重新获取/绑定 IPV4 地址，且 station 的 IPV4 重置为 0。
  - DHCP 客户端重新绑定了其它地址。
  - 静态配置的 IPV4 地址已发生改变。

函数 ``ip_event_got_ip_t`` 中的字段 ``ip_change`` 说明了 IPV4 地址是否发生改变。

套接字的状态是基于 IPV4 地址的，这意味着，如果 IPV4 地址发生改变，则所有与此 IPV4 相关的套接字都将变为异常。接收到此事件后，应用程序需关闭所有套接字，并在 IPV4 变为有效地址时重新创建应用程序。

IP_EVENT_GOT_IP6
++++++++++++++++++++++++++++++++++++

当 IPV6 SLAAC 支持自动为 {IDF_TARGET_NAME} 配置一个地址，或 {IDF_TARGET_NAME} 地址发生改变时，将引发此事件。此事件意味着应用程序一切就绪，可以开始任务（如：创建套接字）。

IP_EVENT_STA_LOST_IP
++++++++++++++++++++++++++++++++++++

当 IPV4 地址失效时，将引发此事件。

此事件不会在 Wi-Fi 断连后立刻出现。Wi-Fi 连接断开后，首先将启动一个 IPV4 地址丢失计时器（可通过 :ref:`CONFIG_ESP_NETIF_LOST_IP_TIMER_ENABLE` 与 :ref:`CONFIG_ESP_NETIF_IP_LOST_TIMER_INTERVAL` 配置）。如果 station 在该计时器超时之前成功获取了 IPV4 地址，则不会发生此事件。否则，此事件将在计时器超时时发生。

一般来说，应用程序可忽略此事件。这只是一个调试事件，主要使应用程序获知 IPV4 地址已丢失。

WIFI_EVENT_AP_START
++++++++++++++++++++++++++++++++++++

与 `WIFI_EVENT_STA_START`_ 事件相似。

WIFI_EVENT_AP_STOP
++++++++++++++++++++++++++++++++++++

与 `WIFI_EVENT_STA_STOP`_ 事件相似。

WIFI_EVENT_AP_STACONNECTED
++++++++++++++++++++++++++++++++++++

每当有一个 station 成功连接 {IDF_TARGET_NAME} AP 时，将引发此事件。接收到此事件后，事件任务将不做任何响应，应用程序的回调函数也可忽略这一事件。但是，你可以在此时进行一些操作，例如：获取已连接 station 的信息等。

WIFI_EVENT_AP_STADISCONNECTED
++++++++++++++++++++++++++++++++++++

此事件将在以下情况下发生：

  - 应用程序通过调用函数 :cpp:func:`esp_wifi_disconnect()` 或 :cpp:func:`esp_wifi_deauth_sta()` 手动断开 station 连接。
  - Wi-Fi 驱动程序出于某些原因断开 station 连接，例如：AP 在过去 5 分钟（可通过函数 :cpp:func:`esp_wifi_set_inactive_time()` 修改该时间）内未接收到任何数据包等。
  - station 断开与 AP 之间的连接。

发生此事件时，事件任务将不做任何响应，但应用程序的事件回调函数需执行一些操作，例如：关闭与此 station 相关的套接字等。

WIFI_EVENT_AP_PROBEREQRECVED
++++++++++++++++++++++++++++++++++++

默认情况下，此事件处于禁用状态，应用程序可以通过调用 API :cpp:func:`esp_wifi_set_event_mask()` 启用。
启用后，每当 AP 接收到 probe request 时都将引发此事件。

.. _wifi-event-sta-beacon-timeout:

WIFI_EVENT_STA_BEACON_TIMEOUT
++++++++++++++++++++++++++++++++++++

如果 station 在 inactive 时间内未收到所连接 AP 的 beacon，将发生 beacon 超时，将引发此事件。inactive 时间通过调用函数 :cpp:func:`esp_wifi_set_inactive_time()` 设置。

.. _wifi-event-connectionless-module-wake-interval-start:

WIFI_EVENT_CONNECTIONLESS_MODULE_WAKE_INTERVAL_START
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++

非连接模块在 `Interval` 开始时触发此事件。 请参考 :ref:`非连接模块功耗管理 <connectionless-module-power-save-cn>` 。



{IDF_TARGET_MAX_CONN_STA_NUM:default="15", esp32c2="4", esp32c3="10", esp32c6="10"}

{IDF_TARGET_SUB_MAX_NUM_FROM_KEYS:default="2", esp32c3="7", esp32c6="7"}

.. _wifi-configuration:

{IDF_TARGET_NAME} Wi-Fi 配置
-------------------------------------

使能 Wi-Fi NVS 时，所有配置都将存储到 flash 中；反之，请参阅 :ref:`Wi-Fi NVS Flash <wifi-nvs-flash>`。

Wi-Fi 模式
+++++++++++++++++++++++++
调用函数 :cpp:func:`esp_wifi_set_mode()` 设置 Wi-Fi 模式。

.. list-table::
   :header-rows: 1
   :widths: 15 50

   * - 模式
     - 描述
   * - ``WIFI_MODE_NULL``
     - NULL 模式：此模式下，内部数据结构不分配给 station 和 AP，同时，station 和 AP 接口不会为发送/接收 Wi-Fi 数据进行初始化。通常，此模式用于 Sniffer，或者你不想通过调用函数 :cpp:func:`esp_wifi_deinit()` 卸载整个 Wi-Fi 驱动程序来同时停止 station 和 AP。
   * - ``WIFI_MODE_STA``
     - station 模式：此模式下，:cpp:func:`esp_wifi_start()` 将初始化内部 station 数据，同时 station 接口准备发送/接收 Wi-Fi 数据。调用函数 :cpp:func:`esp_wifi_connect()` 后，station 将连接到目标 AP。
   * - ``WIFI_MODE_AP``
     - AP 模式：在此模式下，:cpp:func:`esp_wifi_start()` 将初始化内部 AP 数据，同时 AP 接口准备发送/接收 Wi-Fi 数据。随后，Wi-Fi 驱动程序开始广播 beacon，AP 即可与其它 station 连接。
   * - ``WIFI_MODE_APSTA``
     - station/AP 共存模式：在此模式下，函数 :cpp:func:`esp_wifi_start()` 将同时初始化 station 和 AP。该步骤在 station 模式和 AP 模式下完成。请注意 ESP station 所连外部 AP 的信道优先于 ESP AP 信道。


.. only:: esp32c5

    Wi-Fi 频段配置
    +++++++++++++++++++++++++

    可通过函数 :cpp:func:`esp_wifi_set_band_mode()` 设置 {IDF_TARGET_NAME} 所使用的 Wi-Fi 频段模式。

    .. list-table::
        :header-rows: 1
        :widths: 15 50

        * - 模式
          - 描述
        * - ``WIFI_BAND_MODE_2G_ONLY``
          - **2.4 GHz 频段模式**：设备仅可在 2.4 GHz 频段信道上工作。
        * - ``WIFI_BAND_MODE_5G_ONLY``
          - **5 GHz 频段模式**：设备仅可在 5 GHz 频段信道上工作。
        * - ``WIFI_BAND_MODE_AUTO``
          - **2.4 GHz + 5 GHz 自动模式**：设备可根据所连接的 AP 或 SoftAP 配置，自动选择 2.4 GHz 或 5 GHz 频段。

    .. note::
        - ``WIFI_BAND_MODE_AUTO`` 并不表示同时支持双频段，而是允许自动选择频段。

        当工作在 ``WIFI_BAND_MODE_AUTO`` 模式下时，可以分别为 2.4 GHz 和 5 GHz 频段配置协议和带宽：

        - 使用函数 :cpp:func:`esp_wifi_set_protocols()` 设置每个频段支持的协议类型（例如 802.11b/g/n/ac/ax）；

        - 使用函数 :cpp:func:`esp_wifi_set_bandwidths()` 设置每个频段的带宽（如 20MHz、40MHz等）。

.. only:: esp32c5

    AP 选择
    +++++++++++++++++++++++++

    当设备扫描到多个 SSID 相同的 AP（接入点）时，{IDF_TARGET_NAME} 会根据信号强度（RSSI）以及频段等信息，选择最合适的 AP 进行连接。默认策略通常优先选择 RSSI 更高的 AP，但在 2.4 GHz 与 5 GHz 并存的环境下，这可能导致设备偏向连接 2.4 GHz 频段，忽略 5 GHz 频段的高性能优势。

    为此，ESP-IDF 提供了 :cpp:type:`wifi_scan_threshold_t` 结构体中的 :cpp:member:`rssi_5g_adjustment` 字段，用于优化 5G AP 的选择优先级。

    .. list-table::
      :header-rows: 1
      :widths: 30 70

      * - 字段
        - 描述
      * - ``rssi_5g_adjustment``
        - 用于在 SSID 相同的 2.4 GHz 和 5 GHz AP 之间进行优先级调整。默认值为 ``10``，表示当 5 GHz AP 的 RSSI 至少比 2.4 GHz AP 低 ``10 dB`` 以内时，优先连接 5 GHz AP。通过合理设置该参数，可以在信号强度接近时优先连接具有更好带宽与干扰抵抗能力的 5 GHz 网络。

    示例：

    若设备扫描到以下两个 AP，SSID 均为 "MyWiFi"：

    - 2.4 GHz AP：RSSI = -60 dBm
    - 5 GHz AP：RSSI = -68 dBm

    则由于 ``rssi_5g_adjustment = 10``（默认值）， 且满足 ``-68 > -60 - 10`` ，设备会优先连接 5 GHz AP。

    .. note::

        该参数仅在扫描结果中存在相同 SSID 且分别处于 2.4 GHz 与 5 GHz 频段的 AP 时生效。其目的是避免设备总是连接信号强度略高但性能较差的 2.4 GHz 网络。


.. _station-basic-configuration:

Station 基本配置
+++++++++++++++++++++++++++++++++++++

API :cpp:func:`esp_wifi_set_config()` 可用于配置 station。配置的参数信息会保存到 NVS 中。下表详细介绍了各个字段。

.. list-table::
   :header-rows: 1
   :widths: 15 45

   * - 字段
     - 描述
   * - ssid
     - station 想要连接的目标 AP 的 SSID。
   * - password
     - 目标 AP 的密码。
   * - scan_method
     - ``WIFI_FAST_SCAN`` 模式下，扫描到一个匹配的 AP 时即结束。 ``WIFI_ALL_CHANNEL_SCAN`` 模式下，在所有信道扫描所有匹配的 AP。默认扫描模式是 ``WIFI_FAST_SCAN``。
   * - bssid_set
     - 如果 bssid_set 为 0，station 连接 SSID 与 “ssid” 字段相同的 AP，同时忽略字段 “bssid”。其他情况下，station 连接 SSID 与 “ssid” 字段相同、BSSID 与 “bssid” 字段也相同的 AP。
   * - bssid
     - 只有当 bssid_set 为 1 时有效。见字段 “bssid_set”。
   * - channel
     - 该字段为 0 时，station 扫描信道 1 ~ N 寻找目标 AP；否则，station 首先扫描值与 “channel” 字段相同的信道，再扫描其他信道。比如，当该字段设置为 3 时，扫描顺序为 3，1，2，...，N 。如果你不知道目标 AP 在哪个信道，请将该字段设置为 0。
   * - sort_method
     - 该字段仅用于 ``WIFI_ALL_CHANNEL_SCAN`` 模式。

       如果设置为 ``WIFI_CONNECT_AP_BY_SIGNAL``，所有匹配的 AP 将会按照信号强度排序，信号最好的 AP 会被首先连接。比如，如果 station 想要连接 ssid 为 “apxx” 的 AP，且扫描到两个这样的 AP。第一个 AP 的信号为 -90 dBm，第二个 AP 的信号为 -30 dBm，station 首先连接第二个 AP。除非失败，才会连接第一个。

       如果设置为 ``WIFI_CONNECT_AP_BY_SECURITY``，所有匹配的 AP 将会按照安全性排序。比如，如果 station 想要连接 ssid 为 “apxx” 的 AP，并且扫描到两个这样的 AP。第一个 AP 为开放式，第二个 AP 为 WPA2 加密，station 首先连接第二个 AP。除非失败，才会连接第一个。
   * - threshold
     - 该字段用来筛选找到的 AP，如果 AP 的 RSSI 或安全模式小于配置的阈值，则不会被连接。

       如果 RSSI 设置为 0，则表示默认阈值，默认 RSSI 阈值为 -127 dBm。如果 authmode 阈值设置为 0，则表示默认阈值，默认 authmode 阈值为开放模式。


.. attention::

    WEP/WPA 安全模式在 IEEE802.11-2016 协议中已弃用，建议不要使用。可使用 authmode 阈值代替，通过将 threshold.authmode 设置为 ``WIFI_AUTH_WPA2_PSK`` 使用 WPA2 模式。

AP 基本配置
+++++++++++++++++++++++++++++++++++++

API :cpp:func:`esp_wifi_set_config()` 可用于配置 AP。配置的参数信息会保存到 NVS 中。下表详细介绍了各个字段。

.. only:: esp32 or esp32s2 or esp32s3 or esp32c3 or esp32c5 or esp32c6

    .. list-table::
      :header-rows: 1
      :widths: 15 55

      * - 字段
        - 描述
      * - ssid
        - 指 AP 的 SSID。如果 ssid[0] 和 ssid[1] 均为 0xFF，AP 默认 SSID 为 ``ESP_aabbcc``，”aabbcc” 是 AP MAC 的最后三个字节。
      * - password
        - AP 的密码。如果身份验证模式为 ``WIFI_AUTH_OPEN``，此字段将被忽略。
      * - ssid_len
        - SSID 的长度。如果 ssid_len 为 0，则检查 SSID 直至出现终止字符。如果 ssid_len 大于 32，请更改为 32，或者根据 ssid_len 设置 SSID 长度。
      * - channel
        - AP 的信道。如果信道超出范围，Wi-Fi 驱动程序将返回 error。所以，请确保信道在要求的范围内。有关详细信息，请参阅 `Wi-Fi 国家/地区代码`_。
      * - authmode
        - ESP AP 的身份验证模式。目前，ESP AP 不支持 AUTH_WEP。如果 authmode 是一个无效值，AP 默认该值为 ``WIFI_AUTH_OPEN``。
      * - ssid_hidden
        - 如果 ssid_hidden 为 1，AP 不广播 SSID。若为其他值，则广播。
      * - max_connection
        - 允许连接 station 的最大数目，默认值是 10。ESP Wi-Fi 支持 {IDF_TARGET_MAX_CONN_STA_NUM} (``ESP_WIFI_MAX_CONN_NUM``) 个 Wi-Fi 连接。请注意， ESP AP 和 ESP-NOW 共享同一块加密硬件 keys，因此 max_connection 参数将受到 :ref:`CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM` 的影响。加密硬件 keys 的总数是 17，如果 :ref:`CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM` 小于等于 {IDF_TARGET_SUB_MAX_NUM_FROM_KEYS}，那么 max_connection 最大可以设置为 {IDF_TARGET_MAX_CONN_STA_NUM}，否则 max_connection 最大可以设置为 (17 - :ref:`CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM`)。
      * - beacon_interval
        - beacon 间隔。值为 100 ~ 60000 ms，默认值为 100 ms。如果该值不在上述范围，AP 默认取 100 ms。


.. only:: esp32c2

    .. list-table::
      :header-rows: 1
      :widths: 15 55

      * - 字段
        - 描述
      * - ssid
        - 指 AP 的 SSID。如果 ssid[0] 和 ssid[1] 均为 0xFF，AP 默认 SSID 为 ``ESP_aabbcc``，”aabbcc” 是 AP MAC 的最后三个字节。
      * - password
        - AP 的密码。如果身份验证模式为 ``WIFI_AUTH_OPEN``，此字段将被忽略。
      * - ssid_len
        - SSID 的长度。如果 ssid_len 为 0，则检查 SSID 直至出现终止字符。如果 ssid_len 大于 32，请更改为 32，或者根据 ssid_len 设置 SSID 长度。
      * - channel
        - AP 的信道。如果信道超出范围，Wi-Fi 驱动程序将默认为信道 1。所以，请确保信道在要求的范围内。有关详细信息，请参阅 `Wi-Fi 国家/地区代码`_。
      * - authmode
        - ESP AP 的身份验证模式。目前，ESP AP 不支持 AUTH_WEP。如果 authmode 是一个无效值，AP 默认该值为 ``WIFI_AUTH_OPEN``。
      * - ssid_hidden
        - 如果 ssid_hidden 为 1，AP 不广播 SSID。若为其他值，则广播。
      * - max_connection
        - 允许连接 station 的最大数目，默认值是 2。ESP Wi-Fi 支持 {IDF_TARGET_MAX_CONN_STA_NUM} (``ESP_WIFI_MAX_CONN_NUM``) 个 Wi-Fi 连接。请注意， ESP AP 和 ESP-NOW 共享同一块加密硬件 keys，因此 max_connection 参数将受到 :ref:`CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM` 的影响。加密硬件 keys 的总数是 {IDF_TARGET_MAX_CONN_STA_NUM}， max_connection 最大可以设置为 ({IDF_TARGET_MAX_CONN_STA_NUM} - :ref:`CONFIG_ESP_WIFI_ESPNOW_MAX_ENCRYPT_NUM`)。
      * - beacon_interval
        - beacon 间隔。值为 100 ~ 60000 ms，默认值为 100 ms。如果该值不在上述范围，AP 默认取 100 ms。


Wi-Fi 协议模式
+++++++++++++++++++++++++

目前，IDF 支持以下协议模式：

.. only:: esp32 or esp32s2 or esp32c3 or esp32s3

    .. list-table::
      :header-rows: 1
      :widths: 15 55

      * - 协议模式
        - 描述
      * - 802.11b
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B)``，将 station/AP 设置为仅 802.11b 模式。
      * - 802.11bg
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G)``，将 station/AP 设置为 802.11bg 模式。
      * - 802.11g
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G)`` 和 ``esp_wifi_config_11b_rate(ifx, true)``，将 station/AP 设置为 802.11g 模式。
      * - 802.11bgn
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)``，将 station/AP 设置为 802.11bgn 模式。
      * - 802.11gn
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)`` 和 ``esp_wifi_config_11b_rate(ifx, true)``，将 station/AP 设置为 802.11gn 模式。
      * - 802.11 BGNLR
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR)``，将 station/AP 设置为 802.11bgn 和 LR 模式。
      * - 802.11 LR
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_LR)``，将 station/AP 设置为 LR 模式。

          **此模式是乐鑫的专利模式，可以达到 1 公里视线范围。请确保 station 和 AP 同时连接至 ESP 设备。**

.. only:: esp32c6

    .. list-table::
      :header-rows: 1
      :widths: 15 55

      * - 协议模式
        - 描述
      * - 802.11b
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B)``，将 station/AP 设置为仅 802.11b 模式。
      * - 802.11bg
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G)``，将 station/AP 设置为 802.11bg 模式。
      * - 802.11g
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G)`` 和 ``esp_wifi_config_11b_rate(ifx, true)``，将 station/AP 设置为 802.11g 模式。
      * - 802.11bgn
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)``，将 station/AP 设置为 802.11bgn 模式。
      * - 802.11gn
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)`` 和 ``esp_wifi_config_11b_rate(ifx, true)``，将 station/AP 设置为 802.11gn 模式。
      * - 802.11 BGNLR
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR)``，将 station/AP 设置为 802.11bgn 和 LR 模式。
      * - 802.11bgnax
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_11AX)``，将 station/AP 设置为 802.11bgnax 模式。
      * - 802.11 BGNAXLR
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_11AX|WIFI_PROTOCOL_LR)``，将 station/AP 设置为 802.11bgnax 和 LR 模式。
      * - 802.11 LR
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_LR)``，将 station/AP 设置为 LR 模式。

          **此模式是乐鑫的专利模式，可以达到 1 公里视线范围。请确保 station 和 AP 同时连接至 ESP 设备。**


.. only:: esp32c5

    - **2.4 GHz 频段**：支持 802.11b、802.11bg、802.11bgn、802.11bgnax 以及乐鑫专有的 LR 模式。
    - **5 GHz 频段**：支持 802.11a、802.11an、802.11anac 和 802.11anacax。

    {IDF_TARGET_NAME} 支持为 2.4 GHz 和 5 GHz 频段分别配置 Wi-Fi 协议模式。推荐使用 :cpp:func:`esp_wifi_set_protocols()` 进行配置，也兼容旧接口 :cpp:func:`esp_wifi_set_protocol()`。

    **推荐用法**

    使用新 API :cpp:func:`esp_wifi_set_protocols()`，分别设置两个频段的协议：

    .. code-block:: c

        // 设置 2.4 GHz 使用 802.11bgnax，5 GHz 使用 802.11anacax 协议
        wifi_protocols_t protocols = {
            .ghz_2g = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AX,
            .ghz_5g = WIFI_PROTOCOL_11A | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AC | WIFI_PROTOCOL_11AX,
        };
        esp_wifi_set_protocols(WIFI_IF_STA, &protocols);

    **兼容用法**

    使用旧 API :cpp:func:`esp_wifi_set_protocol()`，配置 2.4 GHz 或者 5 GHz 频段的协议：

    .. code-block:: c

        // 设置频段为 2.4 GHz 频段
        esp_wifi_set_band_mode(WIFI_BAND_MODE_2G_ONLY);

        // 设置 STA 的协议为 802.11bgnax
        uint8_t protocol = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AX;
        esp_wifi_set_protocol(WIFI_IF_STA, protocol);

    .. note::

        - 新接口 `esp_wifi_set_protocols()` 可同时配置两个频段，建议在{IDF_TARGET_NAME}上优先使用。
        - ``esp_wifi_set_protocol()`` 适用于仅使用单一频段的场景，如 ``WIFI_BAND_MODE_2G_ONLY`` 或 ``WIFI_BAND_MODE_5G_ONLY`` 模式。仅对当前连接的频段生效，例如，当接口处于 5 GHz 频段时，该函数将忽略对 2.4 GHz 频段的配置。
        - 若配置包含不受支持的协议组合，函数调用将返回错误。
        - 如需启用乐鑫专有 LR 模式，请确保 2.4 GHz 配置中包含 `WIFI_PROTOCOL_LR`。


.. only:: esp32c2

    .. list-table::
      :header-rows: 1
      :widths: 15 55

      * - 协议模式
        - 描述
      * - 802.11b
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B)``，将 station/AP 设置为仅 802.11b 模式。
      * - 802.11bg
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G)``，将 station/AP 设置为 802.11bg 模式。
      * - 802.11g
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G)`` 和 ``esp_wifi_config_11b_rate(ifx, true)``，将 station/AP 设置为 802.11g 模式。
      * - 802.11bgn
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)``，将 station/AP 设置为 802.11bgn 模式。
      * - 802.11gn
        - 调用函数 ``esp_wifi_set_protocol(ifx, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)`` 和 ``esp_wifi_config_11b_rate(ifx, true)``，将 station/AP 设置为 802.11gn 模式。


Wi-Fi 带宽模式
++++++++++++++++++++++

.. only:: esp32 or esp32s2 or esp32c3 or esp32s3

    {IDF_TARGET_NAME} 目前支持 20 MHz 和 40 MHz 两种带宽模式，并与协议模式组合使用，常见的组合包括：

    - **HT20**：802.11n，20 MHz 带宽
    - **HT40**：802.11n，40 MHz 带宽

    .. note::

        - 40 MHz 带宽模式仅在 802.11n 模式下支持。

    应用程序可以使用 :cpp:func:`esp_wifi_set_bandwidth()` API 来设置当前接口的带宽模式。

    示例：

    .. code-block:: c

        // 设置 STA 的协议为 802.11bgn
        uint8_t protocol = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N;
        esp_wifi_set_protocol(WIFI_IF_STA, protocol);

        // 设置 STA 的带宽为 40 MHz
        esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW40);

.. only:: esp32c6

    {IDF_TARGET_NAME} 目前支持 20 MHz 和 40 MHz 两种带宽模式，并与协议模式组合使用，常见的组合包括：

    - **HT20**：802.11n，20 MHz 带宽
    - **HT40**：802.11n，40 MHz 带宽
    - **HE20**：802.11ax，20 MHz 带宽

    .. note::

        - 40 MHz 带宽模式仅在 802.11n 模式下支持。

    应用程序可以使用 :cpp:func:`esp_wifi_set_bandwidth()` API 来设置当前接口的带宽模式。

    示例：

    .. code-block:: c

        // 设置 STA 的协议为 802.11bgn
        uint8_t protocol = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N;
        esp_wifi_set_protocol(WIFI_IF_STA, protocol);

        // 设置 STA 的带宽为 40 MHz
        esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW40);

.. only:: esp32c5

    {IDF_TARGET_NAME} 当前支持 20 MHz 和 40 MHz 两种带宽模式，可与不同协议模式组合使用，常见配置包括：

    - **HT20**：802.11n/11an，20 MHz 带宽
    - **HT40**：802.11n/11an，40 MHz 带宽
    - **HE20**：802.11ax，20 MHz 带宽

    .. note::

        - 40 MHz 模式仅适用于 802.11n（2.4 GHz）或 802.11an（5 GHz）协议。

    应用程序可使用 :cpp:func:`esp_wifi_set_bandwidths()` API，分别为 2.4 GHz 和 5 GHz 配置独立带宽。

    示例：

    .. code-block:: c

        // 设置 2.4 GHz 使用 802.11bgnax，5 GHz 使用 802.11an 协议
        wifi_protocols_t protocols = {
            .ghz_2g = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AX,
            .ghz_5g = WIFI_PROTOCOL_11A | WIFI_PROTOCOL_11N,
        };
        esp_wifi_set_protocols(WIFI_IF_STA, &protocols);

        // 设置 2.4 GHz 带宽为 20 MHz，5 GHz 带宽为 40 MHz
        wifi_bandwidths_t bw = {
            .ghz_2g = WIFI_BW20,
            .ghz_5g = WIFI_BW40
        };
        esp_wifi_set_bandwidths(WIFI_IF_STA, &bw);

    .. note::

        - 若 `.ghz_2g` 设置为 0，仅配置 5 GHz 带宽，2.4 GHz 保持不变。
        - 若 `.ghz_5g` 设置为 0，仅配置 2.4 GHz 带宽，5 GHz 保持不变。

    **兼容用法**

    可使用旧版 API :cpp:func:`esp_wifi_set_bandwidth()` 配置 2.4 GHz 频段或者 5 GHz 频段的带宽：

    .. code-block:: c

          // 设置频段为 5 GHz 频段
          esp_wifi_set_band_mode(WIFI_BAND_MODE_5G_ONLY);

          // 设置 STA 的协议为 802.11an
          uint8_t protocol = WIFI_PROTOCOL_11A | WIFI_PROTOCOL_11N;
          esp_wifi_set_protocol(WIFI_IF_STA, protocol);

          // 设置 STA 的带宽为 40 MHz
          esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW40);

    .. note::

        - 新版 API :cpp:func:`esp_wifi_set_bandwidths()` 支持同时配置 2.4 GHz 与 5 GHz 带宽，推荐在 {IDF_TARGET_NAME} 上优先使用。
        - ``esp_wifi_set_bandwidth()`` 适用于单频段场景，如 ``WIFI_BAND_MODE_2G_ONLY`` 或 ``WIFI_BAND_MODE_5G_ONLY``，仅作用于当前连接频段。例如，若接口工作在 5 GHz，则 2.4 GHz 的配置将被忽略。
        - 若设置了当前频段不支持的带宽值，函数将返回错误。

.. only:: esp32c2

    {IDF_TARGET_NAME} 目前仅支持 20 MHz 带宽模式。

.. only:: esp32 or esp32s2 or esp32c3 or esp32s3 or esp32c5 or esp32c6

    长距离 (LR)
    +++++++++++++++++++++++++

    长距离 (LR) 模式是乐鑫的一项专利 Wi-Fi 模式，可达到 1 公里视线范围。与传统 802.11b 模式相比，接收灵敏度更高，抗干扰能力更强，传输距离更长。

    LR 兼容性
    *************************

    由于 LR 是乐鑫的独有 2.4 GHz 频段 Wi-Fi 模式，只有 ESP32 芯片系列设备（ESP32-C2 除外）才能传输和接收 LR 数据。也就是说，如果连接的设备不支持 LR，ESP32 芯片系列设备（ESP32-C2 除外）则不会以 LR 数据速率传输数据。可通过配置适当的 Wi-Fi 模式使你的应用程序实现这一功能。

    - 如果协商的模式支持 LR，ESP32 芯片系列设备（ESP32-C2 除外）可能会以 LR 速率传输数据。
    - 否则，ESP32 芯片系列设备（ESP32-C2 除外）将以传统 Wi-Fi 数据速率传输所有数据。

    下表是 2.4 GHz 频段 Wi-Fi 模式协商：

    .. only:: esp32 or esp32s2 or esp32c3 or esp32s3

        +-------+-----+----+---+-------+------+-----+----+
        |AP\STA | BGN | BG | B | BGNLR | BGLR | BLR | LR |
        +=======+=====+====+===+=======+======+=====+====+
        | BGN   | BGN | BG | B | BGN   | BG   | B   | -  |
        +-------+-----+----+---+-------+------+-----+----+
        | BG    | BG  | BG | B | BG    | BG   | B   | -  |
        +-------+-----+----+---+-------+------+-----+----+
        | B     | B   | B  | B | B     | B    | B   | -  |
        +-------+-----+----+---+-------+------+-----+----+
        | BGNLR | -   | -  | - | BGNLR | BGLR | BLR | LR |
        +-------+-----+----+---+-------+------+-----+----+
        | BGLR  | -   | -  | - | BGLR  | BGLR | BLR | LR |
        +-------+-----+----+---+-------+------+-----+----+
        | BLR   | -   | -  | - | BLR   | BLR  | BLR | LR |
        +-------+-----+----+---+-------+------+-----+----+
        | LR    | -   | -  | - | LR    | LR   | LR  | LR |
        +-------+-----+----+---+-------+------+-----+----+

    .. only:: esp32c5 or esp32c6

        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | AP\STA  | BGNAX | BGN | BG | B | BGNAXLR | BGNLR | BGLR | BLR | LR |
        +=========+=======+=====+====+===+=========+=======+======+=====+====+
        | BGNAX   | BGAX  | BGN | BG | B | BGAX    | BGN   | BG   | B   | -  |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | BGN     | BGN   | BGN | BG | B | BGN     | BGN   | BG   | B   | -  |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | BG      | BG    | BG  | BG | B | BG      | BG    | BG   | B   | -  |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | B       | B     | B   | B  | B | B       | B     | B    | B   | -  |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | BGNAXLR | -     | -   | -  | - | BGAXLR  | BGNLR | BGLR | BLR | LR |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | BGNLR   | -     | -   | -  | - | BGNLR   | BGNLR | BGLR | BLR | LR |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | BGLR    | -     | -   | -  | - | BGLR    | BGLR  | BGLR | BLR | LR |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | BLR     | -     | -   | -  | - | BLR     | BLR   | BLR  | BLR | LR |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+
        | LR      | -     | -   | -  | - | LR      | LR    | LR   | LR  | LR |
        +---------+-------+-----+----+---+---------+-------+------+-----+----+

    上表中，行是 AP 的 Wi-Fi 模式，列是 station 的 Wi-Fi 模式。"-" 表示 AP 和 station 的 Wi-Fi 模式不兼容。

    根据上表，得出以下结论：

    - 对于已使能 LR 的 {IDF_TARGET_NAME} AP，由于以 LR 模式发送 beacon，因此与传统的 802.11 模式不兼容。
    - 对于已使能 LR 且并非仅 LR 模式的 {IDF_TARGET_NAME} station，与传统 802.11 模式兼容。
    - 如果 station 和 AP 都是 ESP32 芯片系列设备（ESP32-C2 除外），并且两者都使能 LR 模式，则协商的模式支持 LR。

    如果协商的 Wi-Fi 模式同时支持传统的 802.11 模式和 LR 模式，则 Wi-Fi 驱动程序会在不同的 Wi-Fi 模式下自动选择最佳数据速率，应用程序无需任何操作。

    LR 对传统 Wi-Fi 设备的影响
    ***************************************

    以 LR 速率进行的数据传输不会影响传统 Wi-Fi 设备，因为：

    - LR 模式下的 CCA 和回退过程符合 802.11 协议。
    - 传统的 Wi-Fi 设备可以通过 CCA 检测 LR 信号并进行回退。

    也就是说，LR 模式下传输效果与 802.11b 模式相似。

    LR 传输距离
    *************************

    LR 的接收灵敏度比传统的 802.11b 模式高 4 dB，理论上，传输距离约为 11B 的 2 至 2.5 倍。

    LR 吞吐量
    *************************

    因为原始 PHY 数据传输速率为 1/2 Mbps 和 1/4 Mbps，LR 的吞吐量有限。

    何时使用 LR
    *************************

    使用 LR 的一般条件如下：

    - AP 和 station 都是乐鑫设备。
    - 需要长距离 Wi-Fi 连接和数据传输。
    - 数据吞吐量要求非常小，例如远程设备控制等。


.. only:: esp32c5

    动态频率选择（DFS）
    +++++++++++++++++++++++++++++++++++++++++++++++++++++++

    在 5 GHz Wi-Fi 频段中，一些信道（如信道 52 ~ 144）与天气雷达等关键通信系统共享频谱。为避免 Wi-Fi 信号对这些系统产生干扰，Wi-Fi 设备在使用这些信道前，必须执行特定的侦测与切换机制，即 **动态频率选择（DFS）**。

    启用 DFS 的设备可以访问更多 5 GHz 信道，从而提高网络容量并减少干扰，尤其适用于高密度部署或需要大带宽的应用场景。不同国家和地区 DFS 信道范围可以参考 :component_file:`esp_wifi/regulatory/esp_wifi_regulatory.txt`

    {IDF_TARGET_NAME} 支持在 5 GHz 频段使用 DFS 信道，但仅支持 **被动雷达检测** 功能。

    .. list-table::
      :header-rows: 1
      :widths: 20 60

      * - 类型
        - 描述
      * - 被动雷达检测 支持
        - 设备在扫描过程中能够监听 DFS 信道，识别并连接工作在 DFS 信道上的接入点（AP）。当目标 AP 在 DFS 信道上检测到雷达并通过信道切换（CSA）切换信道时，{IDF_TARGET_NAME} 会跟随 AP 切换信道。
      * - 主动雷达检测 不支持
        - {IDF_TARGET_NAME} **不支持** 主动雷达检测功能，因此无法在 SoftAP 模式下作为 DFS AP 启动。

    .. note::

        - 在 STA 模式下，{IDF_TARGET_NAME}  可连接至位于 DFS 信道上的 AP，前提是该信道已在扫描中被识别。
        - 在 SoftAP 模式下，{IDF_TARGET_NAME} 不允许使用 DFS 信道作为工作信道，以避免违反无线电监管规定。
        - 在 STA+SoftAP 共存模式下：

          1. 当 STA 连接到位于 DFS 信道的 AP 时，SoftAP 允许通过 CSA (Channel Switch Announcement) 切换至相同的 DFS 信道。
          2. 当 STA 断开连接后，SoftAP 将通过 CSA 切换回非 DFS 信道，确保符合监管要求。

..

.. _wifi-country-code:

Wi-Fi 国家/地区代码
+++++++++++++++++++++++++++

.. only:: esp32 or esp32c2 or esp32s2 or esp32c3 or esp32s3 or esp32c6

    调用 :cpp:func:`esp_wifi_set_country()`，设置国家/地区信息。下表详细介绍了各个字段，请在配置这些字段之前参考当地的 2.4 GHz RF 操作规定。

    .. list-table::
      :header-rows: 1
      :widths: 15 55

      * - 字段
        - 描述
      * - cc[3]
        - 国家/地区代码字符串，此属性标识 station/AP 位于的国家/地区或非国家/地区实体。如果是一个国家/地区，该字符串的前两个八位字节是 ISO/IEC3166-1 中规定的国家/地区两位字母代码。第三个八位字节应是下述之一：

          - ASCII 码空格字符，代表 station/AP 所处国家/地区的规定允许当前频段所需的所有环境。
          - ASCII 码 ‘O’ 字符，代表 station/AP 所处国家/地区的规定仅允许室外环境。
          - ASCII 码 ‘I’ 字符，代表 station/AP 所处国家/地区的规定仅允许室内环境。
          - ASCII 码 ‘X’ 字符，代表 station/AP 位于非国家/地区实体。非国家实体的前两个八位字节是两个 ASCII 码 ‘XX’ 字符。
          - 当前使用的操作类表编号的二进制形式。见 IEEE Std 802.11-2020 附件 E。

      * - schan
        - 起始信道，station/AP 所处国家/地区规定的最小信道值。
      * - nchan
        - 规定的总信道数，比如，如果 schan=1，nchan=13，那么 station/AP 可以从信道 1 至 13 发送数据。
      * - policy
        - 国家/地区策略，当配置的国家/地区信息与所连 AP 的国家/地区信息冲突时，该字段决定使用哪一信息。更多策略相关信息，可参见下文。


    默认国家/地区信息为::

        wifi_country_t config = {
            .cc = "01",
            .schan = 1,
            .nchan = 11,
            .policy = WIFI_COUNTRY_POLICY_AUTO,
        };

    如果 Wi-Fi 模式为 station/AP 共存模式，则它们配置的国家/地区信息相同。有时，station 所连 AP 的国家/地区信息与配置的不同。例如，配置的 station 国家/地区信息为::

        wifi_country_t config = {
            .cc = "JP",
            .schan = 1,
            .nchan = 14,
            .policy = WIFI_COUNTRY_POLICY_AUTO,
        };

    但所连 AP 的国家/地区信息为::

        wifi_country_t config = {
            .cc = "CN",
            .schan = 1,
            .nchan = 13,
        };

    此时，使用所连 AP 的国家/地区信息。

    下表描述了在不同 Wi-Fi 模式和不同国家/地区策略下使用的国家/地区信息，并描述了对主动扫描的影响。

    .. list-table::
      :header-rows: 1
      :widths: 15 15 35

      * - Wi-Fi 模式
        - 策略
        - 描述
      * - station 模式
        - WIFI_COUNTRY_POLICY_AUTO
        - 如果所连 AP 的 beacon 中有国家/地区的 IE，使用的国家/地区信息为 beacon 中的信息，否则，使用默认信息。

          扫描时：

            主动扫描信道 1 至信道 11，被动扫描信道 12 至 信道 14。

          请记住，如果带有隐藏 SSID 的 AP 和 station 被设置在被动扫描信道上，被动扫描将无法找到该 AP。也就是说，如果应用程序希望在每个信道中找到带有隐藏 SSID 的 AP，国家/地区信息应该配置为 WIFI_COUNTRY_POLICY_MANUAL。

      * - station 模式
        - WIFI_COUNTRY_POLICY_MANUAL
        - 总是使用配置的国家/地区信息。

          扫描时：

            主动扫描信道 schan 至信道 schan+nchan-1。

      * - AP 模式
        - WIFI_COUNTRY_POLICY_AUTO
        - 总是使用配置的国家/地区信息。

      * - AP 模式
        - WIFI_COUNTRY_POLICY_MANUAL
        - 总是使用配置的国家/地区信息。

      * - station/AP 共存模式
        - WIFI_COUNTRY_POLICY_AUTO
        - 该 station 与 station 模式、WIFI_COUNTRY_POLICY_AUTO 策略下使用的国家/地区信息相同。
          如果 station 不连接任何外部 AP，AP 使用配置的国家/地区信息。如果 station 连接一个外部 AP，该 AP 的国家/地区信息与该 station 相同。

      * - station/AP 共存模式
        - WIFI_COUNTRY_POLICY_MANUAL
        - 该 station 与 station 模式、WIFI_COUNTRY_POLICY_MANUAL 策略下使用的国家/地区信息相同。
          该 AP 与 AP 模式、WIFI_COUNTRY_POLICY_MANUAL 策略下使用的国家/地区信息相同。

.. only:: esp32c5

    调用 :cpp:func:`esp_wifi_set_country()` 可设置国家/地区信息。下表详细介绍了各字段含义，请在配置这些字段前，参考本地 2.4 GHz 与 5 GHz 射频使用规定。

    .. list-table::
      :header-rows: 1
      :widths: 15 55

      * - 字段
        - 描述
      * - cc[3]
        - 国家/地区代码字符串。用于标识 station/AP 所在的国家/地区，或非国家实体。如果为国家/地区，该字段的前两个字节为符合 ISO/IEC 3166-1 标准的两位国家代码，第三个字节含义如下：

          - 空格字符（ASCII），表示当前国家/地区在所有环境中允许使用相应频段。
          - 字符 `'O'`，表示仅允许在户外环境中使用。
          - 字符 `'I'`，表示仅允许在室内环境中使用。
          - 字符 `'X'`，表示非国家实体，此时前两个字符应为 `'X'`（即 `'XX'`）。
          - 二进制形式的操作类编号，参考 IEEE Std 802.11-2020 附录 E。

      * - schan
        - 起始信道值，即当前国家/地区在 2.4 GHz 频段中允许使用的最小信道。
      * - nchan
        - 信道数量。定义在 2.4 GHz 频段中允许使用的总信道数。例如，若 schan = 1，nchan = 13，则 station/AP 可使用信道 1 至 13。
      * - policy
        - 国家/地区策略。当配置的国家/地区与连接 AP 的信息冲突时，此字段决定应使用哪一方的信息。详情见下文说明。
      * - wifi_5g_channel_mask
        - 表示 station/AP 在 5 GHz 频段中可使用的信道掩码。信道值与位的对应关系详见 :cpp:enum:`wifi_5g_channel_bit_t`。

    默认配置示例如下::

        wifi_country_t config = {
            .cc = "01",
            .schan = 1,
            .nchan = 11,
            .wifi_5g_channel_mask = 0xfe,
            .policy = WIFI_COUNTRY_POLICY_AUTO,
        };

    若 Wi-Fi 处于 station/AP 共存模式，则两者使用相同的国家/地区信息。有时，连接的 AP 的国家/地区信息可能与 station 预设的不同。例如：

    配置的 station 信息为::

        wifi_country_t config = {
            .cc = "JP",
            .schan = 1,
            .nchan = 14,
            .wifi_5g_channel_mask = 0xfe,
            .policy = WIFI_COUNTRY_POLICY_AUTO,
        };

    而连接的 AP 信息为::

        wifi_country_t config = {
            .cc = "CN",
            .schan = 1,
            .nchan = 13,
        };

    此时将使用连接的 AP 的国家/地区信息。

    下表说明了在不同 Wi-Fi 模式及策略下使用的国家/地区信息，并说明了扫描行为的差异：

    .. list-table::
      :header-rows: 1
      :widths: 15 15 35

      * - Wi-Fi 模式
        - 策略
        - 描述
      * - station 模式
        - WIFI_COUNTRY_POLICY_AUTO
        - 若连接的 AP 的 beacon 中包含国家/地区信息 IE，则使用该信息；否则使用默认配置。

          扫描行为：

            - 2.4 GHz 频段：主动扫描信道 1–11，被动扫描信道 12–14；
            - 5 GHz 频段：主动扫描非 DFS 信道，被动扫描 DFS 信道。

          注意：若隐藏 SSID 的 AP 设在被动扫描信道上，扫描可能无法发现该 AP。若需在所有信道上发现隐藏 SSID，请使用 `WIFI_COUNTRY_POLICY_MANUAL`。

      * - station 模式
        - WIFI_COUNTRY_POLICY_MANUAL
        - 始终使用配置的国家/地区信息。

          扫描行为：

            - 2.4 GHz 频段：扫描从 `schan` 到 `schan + nchan - 1` 的信道；
            - 5 GHz 频段：扫描 `wifi_5g_channel_mask` 中标记支持的信道。

      * - AP 模式
        - WIFI_COUNTRY_POLICY_AUTO
        - 始终使用配置的国家/地区信息。
      * - AP 模式
        - WIFI_COUNTRY_POLICY_MANUAL
        - 始终使用配置的国家/地区信息。
      * - station/AP 共存模式
        - WIFI_COUNTRY_POLICY_AUTO
        - 与 station 模式中的 `WIFI_COUNTRY_POLICY_AUTO` 相同。

          若 station 未连接任何 AP，则 AP 使用配置的国家/地区信息；
          若 station 连接了外部 AP，则 AP 使用该 station 获取的国家/地区信息。

      * - station/AP 共存模式
        - WIFI_COUNTRY_POLICY_MANUAL
        - 与 station 模式中 `WIFI_COUNTRY_POLICY_MANUAL` 相同，AP 也始终使用配置的国家/地区信息。


主信道
*************************

AP 模式下，AP 的信道定义为主信道。station 模式下，station 所连 AP 的信道定义为主信道。station/AP 共存模式下，AP 和 station 的主信道必须相同。如果不同，station 的主信道始终优先。比如，初始时，AP 位于信道 6，但 station 连接信道 9 的 AP。因为 station 的主信道具有优先性，该 AP 需要将信道从 6 切换至 9，确保与 station 主信道相同。切换信道时，AP 模式下的 {IDF_TARGET_NAME} 将使用信道切换公告 (CSA) 通知连接的 station。支持信道切换的 station 将直接通过，无需与 AP 断连再重新连接。


.. _wifi-menuconfig:

Wi-Fi Menuconfig
-----------------------

.. _wifi-buffer-configure:

Wi-Fi 缓冲区配置
+++++++++++++++++++++++

如果要修改默认的缓冲区数量或类型，最好也了解缓冲区在数据路径中如何分配或释放。下图显示了发送数据方向的过程。

.. blockdiag::
    :caption: TX Buffer Allocation
    :align: center

    blockdiag buffer_allocation_tx {

        # global attributes
        node_height = 60;
        node_width = 100;
        span_width = 50;
        span_height = 20;
        default_shape = roundedbox;

        # labels of diagram nodes
        APPL_TASK [label="Application\n task", fontsize=12];
        LwIP_TASK [label="LwIP\n task", fontsize=12];
        WIFI_TASK [label="Wi-Fi\n task", fontsize=12];

        # labels of description nodes
        APPL_DESC [label="1> User data", width=120, height=25, shape=note, color=yellow];
        LwIP_DESC [label="2> Pbuf", width=120, height=25, shape=note, color=yellow];
        WIFI_DESC [label="3> Dynamic (Static)\n TX Buffer", width=150, height=40, shape=note, color=yellow];

        # node connections
        APPL_TASK -> LwIP_TASK -> WIFI_TASK
        APPL_DESC -> LwIP_DESC -> WIFI_DESC [style=none]
    }


描述：

 - 应用程序分配需要发送的数据。
 - 应用程序调用 TCPIP 或套接字相关的 API 发送用户数据。这些 API 会分配一个在 LwIP 中使用的 PBUF，并复制用户数据。
 - 当 LwIP 调用 Wi-Fi API 发送 PBUF 时，Wi-Fi API 会分配一个“动态发送数据缓冲区”或“静态发送数据缓冲区”，并复制 LwIP PBUF，最后发送数据。

下图展示了如何在接收数据方向分配或释放缓冲区：

.. blockdiag::
    :caption: 接收数据缓冲区分配
    :align: center

    blockdiag buffer_allocation_rx {

        # global attributes
        node_height = 60;
        node_width = 100;
        span_width = 40;
        span_height = 20;
        default_shape = roundedbox;

        # labels of diagram nodes
        APPL_TASK [label="Application\n task", fontsize=12];
        LwIP_TASK [label="LwIP\n task", fontsize=12];
        WIFI_TASK [label="Wi-Fi\n task", fontsize=12];
        WIFI_INTR [label="Wi-Fi\n interrupt", fontsize=12];

        # labels of description nodes
        APPL_DESC [label="4> User\n Data Buffer", height=40, shape=note, color=yellow];
        LwIP_DESC [label="3> Pbuf", height=40, shape=note, color=yellow];
        WIFI_DESC [label="2> Dynamic\n RX Buffer", height=40, shape=note, color=yellow];
        INTR_DESC [label="1> Static\n RX Buffer", height=40, shape=note, color=yellow];

        # node connections
        APPL_TASK <- LwIP_TASK <- WIFI_TASK <- WIFI_INTR
        APPL_DESC <- LwIP_DESC <- WIFI_DESC <- INTR_DESC [style=none]
    }

描述：

 - Wi-Fi 硬件在空中接收到数据包后，将数据包内容放到“静态接收数据缓冲区”，也就是“接收数据 DMA 缓冲区”。
 - Wi-Fi 驱动程序分配一个“动态接收数据缓冲区”、复制“静态接收数据缓冲区”，并将“静态接收数据缓冲区”返回给硬件。
 - Wi-Fi 驱动程序将数据包传送到上层 (LwIP)，并分配一个 PBUF 用于存放“动态接收数据缓冲区”。
 - 应用程序从 LwIP 接收数据。

下表是 Wi-Fi 内部缓冲区的配置情况。

.. list-table::
   :header-rows: 1
   :widths: 10 10 10 10 25

   * - 缓冲区类型
     - 分配类型
     - 默认
     - 是否可配置
     - 描述
   * - 静态接收数据缓冲区（硬件接收数据缓冲区）
     - 静态
     - 10 * 1600 Bytes
     - 是
     - 这是一种 DMA 内存，在函数 :cpp:func:`esp_wifi_init()` 中初始化，在函数 :cpp:func:`esp_wifi_deinit()` 中释放。 该缓冲区形成硬件接收列表。当通过空中接收到一个帧时，硬件将该帧写入缓冲区，并向 CPU 发起一个中断。然后，Wi-Fi 驱动程序从缓冲区中读取内容，并将缓冲区返回到列表中。

       如果应用程序希望减少 Wi-Fi 静态分配的内存，可以将该值从 10 减少到 6， 从而节省 6400 Bytes 的内存。除非禁用 AMPDU 功能，否则不建议将该值降低到 6 以下。
   * - 动态接收数据缓冲区
     - 动态
     - 32
     - 是
     - 缓冲区的长度可变，取决于所接收帧的长度。当 Wi-Fi 驱动程序 从“硬件接收数据缓冲区”接收到一帧时，需要从堆中分配“动态接收数据缓冲区”。在 Menuconfig 中配置的“动态接收数据缓冲区” 数量用来限制未释放的“动态接收数据缓冲区”总数量。
   * - 动态发送数据缓冲区
     - 动态
     - 32
     - 是
     - 这是一种 DMA 内存，位于堆内存中。当上层 (LwIP) 向 Wi-Fi 驱动程序发送数据包时，该缓冲区首先分配一个“动态发送数据缓 冲区”，并复制上层缓冲区。

       动态发送数据缓冲区和静态发送数据缓冲区相互排斥。
   * - 静态发送数据缓冲区
     - 静态
     - 16 * 1600 Bytes
     - 是
     - 这是一种 DMA 内存，在函数 :cpp:func:`esp_wifi_init()` 中初始化，在函数 :cpp:func:`esp_wifi_deinit()` 中释放。 当上层 (LwIP) 向 Wi-Fi 驱动程序发送数据包时，该缓冲区首先 分配一个“静态发送数据缓冲区”，并复制上层缓冲区。

       动态发送数据缓冲区和静态发送数据缓冲区相互排斥。

       由于发送数据缓冲区必须是 DMA 缓冲区，所以当使能 PSRAM 时，发送数据缓冲区必须是静态的。
   * - 管理短缓冲区
     - 动态
     - 8
     - 否
     - Wi-Fi 驱动程序的内部缓冲区。
   * - 管理长缓冲区
     - 动态
     - 32
     - 否
     - Wi-Fi 驱动程序的内部缓冲区。
   * - 管理超长缓冲区
     - 动态
     - 32
     - 否
     - Wi-Fi 驱动程序的内部缓冲区。

.. _wifi-nvs-flash:

Wi-Fi NVS Flash
+++++++++++++++++++++

如果使能 Wi-Fi NVS flash，所有通过 Wi-Fi API 设置的 Wi-Fi 配置都会被存储到 flash 中，Wi-Fi 驱动程序在下次开机或重启时将自动加载这些配置。但是，应用程序可视情况禁用 Wi-Fi NVS flash，例如：其配置信息不需要存储在非易失性内存中、其配置信息已安全备份，或仅出于某些调试原因等。


故障排除
---------------

请见 :doc:`../wireshark-user-guide`。

.. toctree::
    :hidden:

    ../wireshark-user-guide
