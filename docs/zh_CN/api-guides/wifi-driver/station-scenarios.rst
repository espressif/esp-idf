Station 场景
==============

:link_to_translation:`en:[English]`

.. _wifi-station-general-scenario:

{IDF_TARGET_NAME} Wi-Fi Station 通用场景
------------------------------------------------

下图为 station 模式下的宏观场景，其中包含不同阶段的具体描述：

.. seqdiag::
    :caption: station 模式下 Wi-Fi 事件场景示例
    :align: center

    seqdiag sample-scenarios-station-mode {
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
        EVENT_TASK <-  WIFI_TASK   [label="3.2> WIFI_EVENT_STA_START"];
        APP_TASK   <-  EVENT_TASK  [label="3.3> WIFI_EVENT_STA_START"];
        === 4. 连接阶段 ===
        APP_TASK   ->  WIFI_TASK   [label="4.1> 连接 Wi-Fi"];
        EVENT_TASK <-  WIFI_TASK   [label="4.2> WIFI_EVENT_STA_CONNECTED"];
        APP_TASK   <- EVENT_TASK   [label="4.3> WIFI_EVENT_STA_CONNECTED"];
        === 5. 获取 IP 阶段 ===
        EVENT_TASK ->  LwIP_TASK   [label="5.1> 启动 DHCP 客户端"];
        EVENT_TASK <-  LwIP_TASK   [label="5.2> IP_EVENT_STA_GOT_IP"];
        APP_TASK   <-  EVENT_TASK  [label="5.3> IP_EVENT_STA_GOT_IP"];
        APP_TASK   ->  APP_TASK    [label="5.4> 套接字相关初始化"];
        === 6. 断开阶段 ===
        EVENT_TASK <-  WIFI_TASK   [label="6.1> WIFI_EVENT_STA_DISCONNECTED"];
        APP_TASK   <-  EVENT_TASK  [label="6.2> WIFI_EVENT_STA_DISCONNECTED"];
        APP_TASK   ->  APP_TASK    [label="6.3> 断开处理"];
        === 7. IP 更改阶段 ===
        EVENT_TASK <-  LwIP_TASK   [label="7.1> IP_EVENT_STA_GOT_IP"];
        APP_TASK   <-  EVENT_TASK  [label="7.2> IP_EVENT_STA_GOT_IP"];
        APP_TASK   ->  APP_TASK    [label="7.3> 套接字错误处理"];
        === 8. 清理阶段 ===
        APP_TASK   ->  WIFI_TASK   [label="8.1> 断开 Wi-Fi 连接"];
        APP_TASK   ->  WIFI_TASK   [label="8.2> 终止 Wi-Fi"];
        APP_TASK   ->  WIFI_TASK   [label="8.3> 清理 Wi-Fi"];
    }


1. Wi-Fi/LwIP 初始化阶段
++++++++++++++++++++++++++++++

 - s1.1：主任务通过调用函数 :cpp:func:`esp_netif_init()` 创建一个 LwIP 核心任务，并初始化 LwIP 相关工作。

 - s1.2：主任务通过调用函数 :cpp:func:`esp_event_loop_create()` 创建一个系统事件任务，并初始化应用程序事件的回调函数。在此情况下，该回调函数唯一的动作就是将事件中继到应用程序任务中。

 - s1.3：主任务通过调用函数 :cpp:func:`esp_netif_create_default_wifi_ap()` 或 :cpp:func:`esp_netif_create_default_wifi_sta()` 创建有 TCP/IP 堆栈的默认网络接口实例绑定 station 或 AP。

 - s1.4：主任务通过调用函数 :cpp:func:`esp_wifi_init()` 创建 Wi-Fi 驱动程序任务，并初始化 Wi-Fi 驱动程序。

 - s1.5：主任务通过调用 OS API 创建应用程序任务。

推荐按照 s1.1 ~ s1.5 的步骤顺序针对基于 Wi-Fi/LwIP 的应用程序进行初始化。但这一顺序 **并非** 强制，你可以在第 s1.1 步创建应用程序任务，然后在该应用程序任务中进行所有其它初始化操作。不过，如果你的应用程序任务依赖套接字，那么在初始化阶段创建应用程序任务可能并不适用。此时，你可以在接收到 IP 后再进行任务创建。

2. Wi-Fi 配置阶段
+++++++++++++++++++++++++++++++

Wi-Fi 驱动程序初始化成功后，可以进入到配置阶段。该场景下，Wi-Fi 驱动程序处于 station 模式。因此，首先你需调用函数 :cpp:func:`esp_wifi_set_mode` (WIFI_MODE_STA) 将 Wi-Fi 模式配置为 station 模式。可通过调用其它 esp_wifi_set_xxx API 进行更多设置，例如：协议模式、国家代码、带宽等。请参阅 :ref:`{IDF_TARGET_NAME} Wi-Fi 配置 <wifi-configuration>`。

一般情况下，我们会在建立 Wi-Fi 连接之前配置 Wi-Fi 驱动程序，但这 **并非** 强制要求。也就是说，只要 Wi-Fi 驱动程序已成功初始化，你可以在任意阶段进行配置。但是，如果你的 Wi-Fi 在建立连接后不需要更改配置，则应先在此阶段完成配置。因为调用配置 API（例如 :cpp:func:`esp_wifi_set_protocol()`）将会导致 Wi-Fi 连接断开，为操作带来不便。

如果 menuconfig 已使能 Wi-Fi NVS flash，则不论当前阶段还是后续的 Wi-Fi 配置信息都将被存储至该 flash 中。那么，当主板上电/重新启动时，就不需从头开始配置 Wi-Fi 驱动程序，只需调用函数 esp_wifi_get_xxx API 获取之前存储的配置信息。当然，如果不想使用之前的配置，你也可以重新配置 Wi-Fi 驱动程序。

3. Wi-Fi 启动阶段
++++++++++++++++++++++++++++++++

 - s3.1：调用函数 :cpp:func:`esp_wifi_start()` 启动 Wi-Fi 驱动程序。
 - s3.2：Wi-Fi 驱动程序将事件 :ref:`WIFI_EVENT_STA_START <wifi-event-sta-start>` 发布到事件任务中，然后，事件任务将执行一些正常操作并调用应用程序的事件回调函数。
 - s3.3：应用程序的事件回调函数将事件 :ref:`WIFI_EVENT_STA_START <wifi-event-sta-start>` 中继到应用程序任务中。此时，推荐调用函数 :cpp:func:`esp_wifi_connect()` 进行 Wi-Fi 连接。当然，你也可以等待在 :ref:`WIFI_EVENT_STA_START <wifi-event-sta-start>` 事件发生后的其它阶段再调用此函数。

4. Wi-Fi 连接阶段
+++++++++++++++++++++++++++++++++

 - s4.1：调用函数 :cpp:func:`esp_wifi_connect()` 后，Wi-Fi 驱动程序将启动内部扫描/连接过程。

 - s4.2：如果内部扫描/连接过程成功，将产生 :ref:`WIFI_EVENT_STA_CONNECTED <wifi-event-sta-connected>` 事件。然后，事件任务将启动 DHCP 客户端服务，最终触发 DHCP 程序。

 - s4.3：在此情况下，应用程序的事件回调函数会将 :ref:`WIFI_EVENT_STA_CONNECTED <wifi-event-sta-connected>` 事件中继到应用程序任务中。通常，应用程序不需进行操作，而你可以执行任何动作，例如：打印日志等。

步骤 s4.2 中 Wi-Fi 连接可能会由于某些原因而失败，例如：密码错误、未找到 AP 等。这种情况下，将引发 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件并提示连接错误原因。有关如何处理中断 Wi-Fi 连接的事件，请参阅下文阶段 6 的描述。

5. Wi-Fi 获取 IP 阶段
+++++++++++++++++++++++++++++++++

 - s5.1：一旦步骤 4.2 中的 DHCP 客户端初始化完成，Wi-Fi 驱动程序将进入 *获取 IP* 阶段。
 - s5.2：如果 Wi-Fi 成功从 DHCP 服务器接收到 IP 地址，则将引发 :ref:`IP_EVENT_STA_GOT_IP <ip-event-sta-got-ip>` 事件，事件任务将执行正常处理。
 - s5.3：应用程序的事件回调函数将事件 :ref:`IP_EVENT_STA_GOT_IP <ip-event-sta-got-ip>` 中继到应用程序任务中。对于那些基于 LwIP 构建的应用程序，此事件较为特殊，因为它意味着应用程序已准备就绪，可以开始任务，例如：创建 TCP/UDP 套接字等。此时较为容易犯的一个错误就是在接收到 :ref:`IP_EVENT_STA_GOT_IP <ip-event-sta-got-ip>` 事件之前就初始化套接字。**切忌在接收到 IP 之前启动任何套接字相关操作。**

6. Wi-Fi 断开阶段
+++++++++++++++++++++++++++++++++

 - s6.1：当 Wi-Fi 因为某些原因（例如：AP 掉电、RSSI 较弱等）连接中断时，将产生 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件。此事件也可能在上文阶段 3 中发生。在这里，事件任务将通知 LwIP 任务清除/移除所有 UDP/TCP 连接。然后，所有应用程序套接字都将处于错误状态。也就是说，:ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件发生时，任何套接字都无法正常工作。
 - s6.2：上述情况下，应用程序的事件回调函数会将 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件中继到应用程序任务中。推荐调用函数 :cpp:func:`esp_wifi_connect()` 重新连接 Wi-Fi，关闭所有套接字，并在必要时重新创建套接字。请参阅 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>`。

7. Wi-Fi IP 更改阶段
++++++++++++++++++++++++++++++++++

 - s7.1：如果 IP 地址发生更改，将引发 :ref:`IP_EVENT_STA_GOT_IP <ip-event-sta-got-ip>` 事件，其中 "ip_change" 被置为 "true"。
 - s7.2：**此事件对应用程序至关重要。这一事件发生时，适合关闭所有已创建的套接字并进行重新创建。**


8. Wi-Fi 清理阶段
++++++++++++++++++++++++++++

 - s8.1：调用函数 :cpp:func:`esp_wifi_disconnect()` 断开 Wi-Fi 连接。
 - s8.2：调用函数 :cpp:func:`esp_wifi_stop()` 终止 Wi-Fi 驱动程序。
 - s8.3：调用函数 :cpp:func:`esp_wifi_deinit()` 清理 Wi-Fi 驱动程序。


.. _wifi-scan:

{IDF_TARGET_NAME} Wi-Fi 扫描
----------------------------------

目前，仅 station 或 station/AP 共存模式支持 :cpp:func:`esp_wifi_scan_start()` API。

扫描类型
+++++++++++++++++++++++++

.. list-table::
   :header-rows: 1
   :widths: 15 50

   * - 模式
     - 描述
   * - 主动扫描
     - 通过发送 probe request 进行扫描。该模式为默认的扫描模式。
   * - 被动扫描
     - 不发送 probe request。跳至某一特定信道并等待 beacon。应用程序可通过 :cpp:type:`wifi_scan_config_t` 中的 scan_type 字段使能被动扫描。
   * - 前端扫描
     - 在 station 模式下 Wi-Fi 未连接时，可进行前端扫描。Wi-Fi 驱动程序决定进行前端扫描还是后端扫描，应用程序无法配置这两种模式。
   * - 后端扫描
     - 在 station 模式或 station/AP 共存模式下 Wi-Fi 已连接时，可进行后端扫描。Wi-Fi 驱动程序决定进行前端扫描还是后端扫描，应用程序无法配置这两种模式。
   * - 全信道扫描
     - 扫描所有信道。:cpp:type:`wifi_scan_config_t` 中的 channel 字段为 0 时，当前模式为全信道扫描。
   * - 特定信道扫描
     - 仅扫描特定的信道。:cpp:type:`wifi_scan_config_t` 中的 channel 字段为 1-14 时，当前模式为特定信道扫描。

上表中的扫描模式可以任意组合，因此共有 8 种不同扫描方式：

 - 全信道后端主动扫描
 - 全信道后端被动扫描
 - 全信道前端主动扫描
 - 全信道前端被动扫描
 - 特定信道后端主动扫描
 - 特定信道后端被动扫描
 - 特定信道前端主动扫描
 - 特定信道前端被动扫描

扫描配置
+++++++++++++++++

扫描类型与其他扫描属性通过函数 :cpp:func:`esp_wifi_scan_start()` 进行配置。下表详细描述了函数 :cpp:type:`wifi_scan_config_t` 各字段信息。

.. list-table::
   :header-rows: 1
   :widths: 15 50

   * - 字段
     - 描述
   * - ssid
     - 如果该字段的值不为 NULL，则仅可扫描到具有相同 SSID 值的 AP。
   * - bssid
     - 如果该字段的值不为 NULL，则仅可扫描到具有相同 BSSID 值的 AP。
   * - channel
     - 如果该字段值为 0，将进行全信道扫描；反之，将针对特定信道进行扫描。
   * - show_hidden
     - 如果该字段值为 0，本次扫描将忽略具有隐藏 SSID 的 AP；反之，这些 AP 也会在扫描时被视为正常 AP。
   * - scan_type
     - 如果该字段值为为 WIFI_SCAN_TYPE_ACTIVE，则本次扫描为主动扫描；反之，将被视为被动扫描。
   * - scan_time
     - 该字段用于控制每个信道的扫描时间。

       被动扫描时，scan_time.passive 字段负责为每个信道指定扫描时间。

       主动扫描时，每个信道的扫描时间如下列表所示。其中，min 代表 scan_time_active_min，max 代表 scan_time_active_max。

       - min=0, max=0：每个信道的扫描时间为 120 ms。
       - min>0, max=0：每个信道的扫描时间为 120 ms。
       - min=0, max>0：每个信道的扫描时间为 ``max`` ms。
       - min>0, max>0：每个信道扫描的最短时间为 ``min`` ms。 如果在这段时间内未找到 AP，将跳转至下一个信道。如这段时间内找到 AP，则该信道的扫描时间为 ``max`` ms。

       如希望提升 Wi-Fi 扫描性能，则可修改上述两个参数。


调用 API :cpp:func:`esp_wifi_set_config()` 可全局配置一些扫描属性，请参阅 :ref:`Station 基本配置 <station-basic-configuration>`。

在所有信道中扫描全部 AP（前端）
+++++++++++++++++++++++++++++++++++++++++++

场景：

.. seqdiag::
    :caption: 所有 Wi-Fi 信道的前端扫描
    :align: center

    seqdiag foreground-scan-all-channels {
        activation = none;
        node_width = 80;
        node_height = 60;
        edge_length = 160;
        span_height = 5;
        default_shape = roundedbox;
        default_fontsize = 12;

        APP_TASK   [label = "App\ntask"];
        EVENT_TASK [label = "Event\ntask"];
        WIFI_TASK  [label = "Wi-Fi\ntask"];

        APP_TASK   ->  WIFI_TASK  [label="1.1 > 配置国家代码"];
        APP_TASK   ->  WIFI_TASK  [label="1.2 > 扫描配置"];
        WIFI_TASK  ->  WIFI_TASK  [label="2.1 > 扫描信道 1"];
        WIFI_TASK  ->  WIFI_TASK  [label="2.2 > 扫描信道 2"];
        WIFI_TASK  ->  WIFI_TASK  [label="..."];
        WIFI_TASK  ->  WIFI_TASK  [label="2.x > 扫描信道 N"];
        EVENT_TASK <-  WIFI_TASK  [label="3.1 > WIFI_EVENT_SCAN_DONE"];
        APP_TASK   <-  EVENT_TASK [label="3.2 > WIFI_EVENT_SCAN_DONE"];
    }


上述场景中描述了全信道前端扫描过程。仅 station 模式支持前端扫描，该模式下 station 未连接任何 AP。前端扫描还是后端扫描完全由 Wi-Fi 驱动程序决定，应用程序无法配置这一模式。

详细描述：

扫描配置阶段
**************************

 - s1.1：如果默认的国家信息有误，调用函数 :cpp:func:`esp_wifi_set_country()` 进行配置。请参阅 :ref:`Wi-Fi 国家/地区代码 <wifi-country-code>`。
 - s1.2：调用函数 :cpp:func:`esp_wifi_scan_start()` 配置扫描信息，可参阅 `扫描配置`_。该场景为全信道扫描，将 SSID/BSSID/channel 设置为 0 即可。


Wi-Fi 驱动程序内部扫描阶段
**************************************

 - s2.1：Wi-Fi 驱动程序切换至信道 1，此时的扫描类型为 WIFI_SCAN_TYPE_ACTIVE，同时发送一个 probe request。反之，Wi-Fi 将等待接收 AP beacon。Wi-Fi 驱动程序将在信道 1 停留一段时间。min/max 扫描时间中定义了 Wi-Fi 在信道 1 中停留的时间长短，默认为 120 ms。
 - s2.2：Wi-Fi 驱动程序跳转至信道 2，并重复进行 s2.1 中的步骤。
 - s2.3：Wi-Fi 驱动程序扫描最后的信道 N，N 的具体数值由步骤 s1.1 中配置的国家代码决定。

扫描完成后事件处理阶段
*********************************

 - s3.1：当所有信道扫描全部完成后，将产生 :ref:`WIFI_EVENT_SCAN_DONE <wifi-event-scan-done>` 事件。
 - s3.2：应用程序的事件回调函数告知应用程序任务已接收到 :ref:`WIFI_EVENT_SCAN_DONE <wifi-event-scan-done>` 事件。调用函数 :cpp:func:`esp_wifi_scan_get_ap_num()` 获取在本次扫描中找到的 AP 数量。然后，分配出足够的条目，并调用函数 :cpp:func:`esp_wifi_scan_get_ap_records()` 获取 AP 记录。请注意，一旦调用 :cpp:func:`esp_wifi_scan_get_ap_records()`，Wi-Fi 驱动程序中的 AP 记录将被释放。但是，请不要在单个扫描完成事件中重复调用两次 :cpp:func:`esp_wifi_scan_get_ap_records()`。反之，如果扫描完成事件发生后未调用 :cpp:func:`esp_wifi_scan_get_ap_records()`，则 Wi-Fi 驱动程序中的 AP 记录不会被释放。因此，请务必确保调用函数 :cpp:func:`esp_wifi_scan_get_ap_records()`，且仅调用一次。

在所有信道上扫描全部 AP（后端）
++++++++++++++++++++++++++++++++++++++++

场景：

.. seqdiag::
    :caption: 所有 Wi-Fi 信道的后端扫描
    :align: center

    seqdiag background-scan-all-channels {
        activation = none;
        node_width = 80;
        node_height = 60;
        edge_length = 160;
        span_height = 5;
        default_shape = roundedbox;
        default_fontsize = 12;

        APP_TASK   [label = "App\ntask"];
        EVENT_TASK [label = "Event\ntask"];
        WIFI_TASK  [label = "Wi-Fi\ntask"];

        APP_TASK   ->  WIFI_TASK  [label="1.1 > 配置国家代码"];
        APP_TASK   ->  WIFI_TASK  [label="1.2 > 扫描配置"];
        WIFI_TASK  ->  WIFI_TASK  [label="2.1 > 扫描信道 1"];
        WIFI_TASK  ->  WIFI_TASK  [label="2.2 > 返回主信道 H"];
        WIFI_TASK  ->  WIFI_TASK  [label="2.3 > 扫描信道 2"];
        WIFI_TASK  ->  WIFI_TASK  [label="2.4 > 返回主信道 H"];
        WIFI_TASK  ->  WIFI_TASK  [label="..."];
        WIFI_TASK  ->  WIFI_TASK  [label="2.x-1 > 扫描信道 N"];
        WIFI_TASK  ->  WIFI_TASK  [label="2.x > 返回主信道 H"];
        EVENT_TASK <-  WIFI_TASK  [label="3.1 > WIFI_EVENT_SCAN_DONE"];
        APP_TASK   <-  EVENT_TASK [label="3.2 > WIFI_EVENT_SCAN_DONE"];
    }

上述场景为一次全信道后端扫描。与 `在所有信道中扫描全部 AP（前端）`_ 相比，全信道后端扫描的不同之处在于：在跳至下一个信道之前，Wi-Fi 驱动程序会先返回主信道停留 30 ms，以便 Wi-Fi 连接有一定的时间发送/接收数据。

在所有信道中扫描特定 AP
+++++++++++++++++++++++++++++++++++++++

场景：

.. seqdiag::
    :caption: 扫描特定的 Wi-Fi 信道
    :align: center

    seqdiag scan-specific-channels {
        activation = none;
        node_width = 80;
        node_height = 60;
        edge_length = 160;
        span_height = 5;
        default_shape = roundedbox;
        default_fontsize = 12;

        APP_TASK   [label = "App\ntask"];
        EVENT_TASK [label = "Event\ntask"];
        WIFI_TASK  [label = "Wi-Fi\ntask"];

        APP_TASK   ->  WIFI_TASK  [label="1.1 > 配置国家代码"];
        APP_TASK   ->  WIFI_TASK  [label="1.2 > 扫描配置"];
        WIFI_TASK  ->  WIFI_TASK  [label="2.1 > 扫描信道 C1"];
        WIFI_TASK  ->  WIFI_TASK  [label="2.2 > 扫描信道 C2"];
        WIFI_TASK  ->  WIFI_TASK  [label="..."];
        WIFI_TASK  ->  WIFI_TASK  [label="2.x > 扫描信道 CN，或找到 AP"];
        EVENT_TASK <-  WIFI_TASK  [label="3.1 > WIFI_EVENT_SCAN_DONE"];
        APP_TASK   <-  EVENT_TASK [label="3.2 > WIFI_EVENT_SCAN_DONE"];
    }

该扫描过程与 `在所有信道中扫描全部 AP（前端）`_ 相似。区别在于：

 - s1.1：在步骤 1.2 中，目标 AP 将配置为 SSID/BSSID。
 - s2.1 ~ s2.N：每当 Wi-Fi 驱动程序扫描某个 AP 时，它将检查该 AP 是否为目标 AP。如果本次扫描类型为 ``WIFI_FAST_SCAN``，且确认已找到目标 AP，则将产生扫描完成事件，同时结束本次扫描；反之，扫描将继续。请注意，第一个扫描的信道可能不是信道 1，因为 Wi-Fi 驱动程序会优化扫描顺序。

如果有多个匹配目标 AP 信息的 AP，例如：碰巧扫描到两个 SSID 为 "ap" 的 AP。如果本次扫描类型为 ``WIFI_FAST_SCAN``，则仅可找到第一个扫描到的 "ap"；如果本次扫描类型为 ``WIFI_ALL_CHANNEL_SCAN``，则两个 "ap“ 都将被找到，且 station 将根据配置规则连接至其需要连接的 "ap"，请参阅 :ref:`Station 基本配置 <station-basic-configuration>`。

你可以在任意信道中扫描某个特定的 AP，或扫描该信道中的所有 AP。这两种扫描过程也较为相似。

.. _scan-in-wifi-connect:

在 Wi-Fi 连接模式下扫描
+++++++++++++++++++++++++

调用函数 :cpp:func:`esp_wifi_connect()` 后，Wi-Fi 驱动程序将首先尝试扫描已配置的 AP。Wi-Fi 连接模式下的扫描过程与 `在所有信道中扫描特定 AP`_ 过程相同，但连接模式下扫描结束后将不会产生扫描完成事件。如果已找到目标 AP，则 Wi-Fi 驱动程序将开始 Wi-Fi 连接；反之，将产生 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件。请参阅 `在所有信道中扫描特定 AP`_。

在阻塞模式下扫描
++++++++++++++++++++

如果函数 :cpp:func:`esp_wifi_scan_start()` 中的 block 参数为 "true"，则扫描将以阻塞模式进行，应用任务会被阻塞，直到扫描完成。阻塞模式下的扫描和正常扫描相似，不同之处在于，阻塞模式下扫描完成之后将不会触发扫描完成事件。

并行扫描
+++++++++++++

有时，可能会有两个应用程序任务同时调用函数 :cpp:func:`esp_wifi_scan_start()`，或者某个应用程序任务在获取扫描完成事件之前再次调用了函数 :cpp:func:`esp_wifi_scan_start()`。这两种情况都有可能会发生。**但是，Wi-Fi 驱动程序并不足以支持多个并行的扫描。因此，应避免上述并行扫描**。随着 {IDF_TARGET_NAME} 的 Wi-Fi 功能不断提升，未来的版本中可能会增加并行扫描支持。

.. _scan-when-wifi-is-connecting:

连接 Wi-Fi 时扫描
+++++++++++++++++++++++++++++++

如果 Wi-Fi 正在连接，则调用函数 :cpp:func:`esp_wifi_scan_start()` 后扫描将立即失败，因为 Wi-Fi 连接优先级高于扫描。如果扫描是因为 Wi-Fi 连接而失败的，此时推荐采取的策略为：等待一段时间后重试。因为一旦 Wi-Fi 连接完成后，扫描将立即成功。

但是，延时重试策略并非万无一失。试想以下场景：

- 如果 station 正在连接一个不存在的 AP，或正在使用错误的密码连接一个 AP，此时将产生事件 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>`。
- 接收到断开连接事件后，应用程序调用函数 :cpp:func:`esp_wifi_connect()` 进行重新连接。
- 而另一个应用程序任务（如，控制任务）调用了函数 :cpp:func:`esp_wifi_scan_start()` 进行扫描。这种情况下，每一次扫描都会立即失败，因为 station 一直处于正在连接状态。
- 扫描失败后，应用程序将等待一段时间后进行重新扫描。

上述场景中的扫描永远不会成功，因为 Wi-Fi 一直处于正在连接过程中。因此，如果你的应用程序也可能发生相似的场景，那么就需要为其配置一个更佳的重新连接策略。例如：

- 应用程序可以定义一个连续重新连接次数的最大值，当重新连接的次数达到这个最大值时，立刻停止重新连接。
- 应用程序可以选择在前 N 次连续重连时立即进行重连，然后延迟一段时间后再次尝试重连。

可以给应用程序定义其特殊的重新连接策略，以防止扫描无法成功。请参阅 :ref:`Wi-Fi 重新连接 <wifi-reconnect>`。

.. _wifi-station-connecting-scenario:

{IDF_TARGET_NAME} Wi-Fi Station 连接场景
------------------------------------------------------------

该场景仅针对在扫描阶段只找到一个目标 AP 的情况，对于多个相同 SSID AP 的情况，请参阅 :ref:`{IDF_TARGET_NAME} Wi-Fi Station 在发现多个 AP 时的连接策略 <wifi-station-connecting-when-multiple-aps-are-found>`。

通常，应用程序无需关心这一连接过程。如感兴趣，可参看下述简介。

场景：

.. seqdiag::
    :caption: Wi-Fi station 连接过程
    :align: center

    seqdiag station-connecting-process {
        activation = none;
        node_width = 80;
        node_height = 60;
        edge_length = 160;
        span_height = 5;
        default_shape = roundedbox;
        default_fontsize = 12;

        EVENT_TASK  [label = "Event\ntask"];
        WIFI_TASK   [label = "Wi-Fi\ntask"];
        AP          [label = "AP"];

        === 1. 扫描阶段 ===
        WIFI_TASK  ->  WIFI_TASK [label="1.1 > 扫描"];
        EVENT_TASK <-  WIFI_TASK [label="1.2 > WIFI_EVENT_STA_DISCONNECTED"];
        === 2. 认证阶段 ===
        WIFI_TASK  ->  AP        [label="2.1 > Auth request"];
        EVENT_TASK <-  WIFI_TASK [label="2.2 > WIFI_EVENT_STA_DISCONNECTED"];
        WIFI_TASK  <-  AP        [label="2.3 > Auth response"];
        EVENT_TASK <-  WIFI_TASK [label="2.4 > WIFI_EVENT_STA_DISCONNECTED"];
        === 3. 关联阶段 ===
        WIFI_TASK  ->  AP        [label="3.1 > Assoc request"];
        EVENT_TASK <-  WIFI_TASK [label="3.2 > WIFI_EVENT_STA_DISCONNECTED"];
        WIFI_TASK  <-  AP        [label="3.3 > Assoc response"];
        EVENT_TASK <-  WIFI_TASK [label="3.4 > WIFI_EVENT_STA_DISCONNECTED"];
        === 4. 四次握手阶段 ===
        EVENT_TASK <-  WIFI_TASK [label="4.1 > WIFI_EVENT_STA_DISCONNECTED"];
        WIFI_TASK  <-  AP        [label="4.2 > 1/4 EAPOL"];
        WIFI_TASK  ->  AP        [label="4.3 > 2/4 EAPOL"];
        EVENT_TASK <-  WIFI_TASK [label="4.4 > WIFI_EVENT_STA_DISCONNECTED"];
        WIFI_TASK  <-  AP        [label="4.5 > 3/4 EAPOL"];
        WIFI_TASK  ->  AP        [label="4.6 > 4/4 EAPOL"];
        EVENT_TASK <-  WIFI_TASK [label="4.7 > WIFI_EVENT_STA_CONNECTED"];
    }


扫描阶段
+++++++++++++++++++++

 - s1.1：Wi-Fi 驱动程序开始在“Wi-Fi 连接”模式下扫描。详细信息请参阅 :ref:`在 Wi-Fi 连接模式下扫描 <scan-in-wifi-connect>`。
 - s1.2：如果未找到目标 AP，将产生 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件，原因代码可能为 ``WIFI_REASON_NO_AP_FOUND``、``WIFI_REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY``、``WIFI_REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD`` 或 ``WIFI_REASON_NO_AP_FOUND_IN_RSSI_THRESHOLD``， 具体取决于 station 的配置。请参阅 `Wi-Fi 原因代码`_。

认证阶段
+++++++++++++++++++++

 - s2.1：发送认证请求数据包并使能认证计时器。
 - s1.2：如果在认证计时器超时之前未接收到认证响应数据包，将产生 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件，且原因代码为 ``WIFI_REASON_AUTH_EXPIRE``。请参阅 `Wi-Fi 原因代码`_。
 - s2.3：接收到认证响应数据包，且认证计时器终止。
 - s2.4：AP 在响应中拒绝认证且产生 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件，原因代码为 ``WIFI_REASON_AUTH_FAIL`` 或为 AP 指定的其它原因。请参阅 `Wi-Fi 原因代码`_。

关联阶段
+++++++++++++++++++++

 - s3.1：发送关联请求并使能关联计时器。
 - s3.2：如果在关联计时器超时之前未接收到关联响应，将产生 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件，且原因代码为 ``WIFI_REASON_DISASSOC_DUE_TO_INACTIVITY``。请参阅 `Wi-Fi 原因代码`_。
 - s3.3：接收到关联响应，且关联计时器终止。
 - s3.4：AP 在响应中拒绝关联且产生 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件，原因代码将在关联响应中指定。请参阅 `Wi-Fi 原因代码`_。


四次握手阶段
++++++++++++++++++++++++++

 - s4.1：使能握手定时器，定时器终止之前未接收到 1/4 EAPOL，此时将产生 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件，且原因代码为 ``WIFI_REASON_HANDSHAKE_TIMEOUT``。请参阅 `Wi-Fi 原因代码`_。
 - s4.2：接收到 1/4 EAPOL。
 - s4.3：station 回复 2/4 EAPOL。
 - s4.4：如果在握手定时器终止之前未接收到 3/4 EAPOL，将产生 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件，且原因代码为 ``WIFI_REASON_HANDSHAKE_TIMEOUT``。请参阅 `Wi-Fi 原因代码`_。
 - s4.5：接收到 3/4 EAPOL。
 - s4.6：station 回复 4/4 EAPOL。
 - s4.7：station 产生 :ref:`WIFI_EVENT_STA_CONNECTED <wifi-event-sta-connected>` 事件。


.. _esp_wifi_reason_code:

Wi-Fi 原因代码
+++++++++++++++++++++

下表罗列了 {IDF_TARGET_NAME} 中定义的原因代码。其中，第一列为 :component_file:`esp_wifi/include/esp_wifi_types.h` 中定义的宏名称。名称中省去了前缀 ``WIFI_REASON``，也就是说，名称 ``UNSPECIFIED`` 实际应为 ``WIFI_REASON_UNSPECIFIED``，以此类推。第二列为原因代码的相应数值，该数值与 IEEE 802.11-2020 第 9.4.1.7 节中的定义一致 。（更多详细信息，请参阅前文描述。）最后一列为原因的描述。原因代码从 200 开始为乐鑫自定义的原因代码，这些代码不属于 IEEE 802.11-2020 标准的一部分。

另请注意，REASON_NO_AP_FOUND_XXX 代码按照重要性递增排列。因此，如果单个接入点（AP）因上述多种原因而导致连接失败，则会报告其中较为重要的原因代码。此外，如果有多个接入点满足标识条件，但它们因上述不同原因均连接失败，则报告的原因代码是因最不重要的原因代码而导致连接失败的接入点的代码，因为这是最接近成功连接的情况。

以下原因代码被重命名为较短的形式以适应表格页面宽度：

- TRANSMISSION_LINK_ESTABLISHMENT_FAILED : TX_LINK_EST_FAILED
- NO_AP_FOUND_W_COMPATIBLE_SECURITY : NO_AP_FOUND_SECURITY
- NO_AP_FOUND_IN_AUTHMODE_THRESHOLD : NO_AP_FOUND_AUTHMODE
- NO_AP_FOUND_IN_RSSI_THRESHOLD : NO_AP_FOUND_RSSI

.. list-table::
   :header-rows: 1
   :widths: 41 10 49
   :class: longtable

   * - 原因代码
     - 数值
     - 描述
   * - UNSPECIFIED
     - 1
     - 出现内部错误，例如：内存已满，内部发送失败，或该原因已被远端接收等。
   * - AUTH_EXPIRE
     - 2
     - 先前的 authentication 已失效。

       对于 ESP station，出现以下情况时将报告该代码：

       - authentication 超时；
       - 从 AP 接收到该代码。

       对于 ESP AP，出现以下情况时将报告该代码：

       - 在过去五分钟之内，AP 未从 station 接收到任何数据包；
       - 由于调用了函数 :cpp:func:`esp_wifi_stop()` 导致 AP 终止；
       - 由于调用了函数 :cpp:func:`esp_wifi_deauth_sta()` 导致 station 的 authentication 取消。
   * - AUTH_LEAVE
     - 3
     - authentication 取消，因为发送端 station 正在离开（或已经离开）。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。
   * - DISASSOC_DUE_TO_INACTIVITY
     - 4
     - 因为 AP 不活跃，association 取消。

       对于 ESP station，出现以下情况时报告该代码：

       - 关联超时。
       - 从 AP 接收到该代码。

   * - ASSOC_TOOMANY
     - 5
     - association 取消，因为 AP 无法同时处理所有当前已关联的 STA。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。

       对于 ESP AP，出现以下情况时将报告该代码：

       - 与 AP 相关联的 station 数量已到达 AP 可支持的最大值。
   * - CLASS2_FRAME_FROM_NONAUTH_STA
     - 6
     - 从一个未认证 station 接收到 class-2 frame。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。

       对于 ESP AP，出现以下情况时将报告该代码：

       - AP 从一个未认证 station 接收到数据包。
   * - CLASS3_FRAME_FROM_NONASSOC_STA
     - 7
     - 从一个未关联 station 接收到的 class-3 frame。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。

       对于 ESP AP，出现以下情况时将报告该代码：

       - AP 从未关联 station 接收到数据包。
   * - ASSOC_LEAVE
     - 8
     - association 取消，因为发送端 station 正在离开（或已经离开）BSS。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。
       - 由于调用 :cpp:func:`esp_wifi_disconnect()` 和其它 API，station 断开连接。
   * - ASSOC_NOT_AUTHED
     - 9
     - station 的 re(association) 请求未被响应 station 认证。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。

       对于 ESP AP，出现以下情况时将报告该代码：

       - AP 从一个已关联，但未认证的 station 接收到数据包。
   * - DISASSOC_PWRCAP_BAD
     - 10
     - association 取消，因为无法接收功率能力 (Power Capability) 元素中的信息。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。
   * - DISASSOC_SUPCHAN_BAD
     - 11
     - association 取消，因为无法接收支持的信道 (Supported Channels) 元素中的信息。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。
   * - BSS_TRANSITION_DISASSOC
     - 12
     - AP 希望切换到另一个 AP，作为 BTM 过程的一部分发送。请注意，当 station 发送 BTM 请求并切换到另一个 AP 时，将报告 ROAMING 原因代码，而不是此代码。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。
   * - IE_INVALID
     - 13
     - 无效元素，即内容不符合 Wi-Fi 协议中帧格式 (Frame formats) 章节所描述标准的元素。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。

       对于 ESP AP，出现以下情况时将报告该代码：

       - AP 解析了一个错误的 WPA 或 RSN IE。
   * - MIC_FAILURE
     - 14
     - 消息完整性代码 (MIC) 出错。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。
   * - 4WAY_HANDSHAKE_TIMEOUT
     - 15
     - 四次握手超时。由于某些历史原因，在 ESP 中该原因代码实为 ``WIFI_REASON_HANDSHAKE_TIMEOUT``。

       对于 ESP station，出现以下情况时报告该代码：

       - 握手超时。
       - 从 AP 接收到该代码。
   * - GROUP_KEY_UPDATE_TIMEOUT
     - 16
     - 组密钥 (Group-Key) 握手超时。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。
   * - IE_IN_4WAY_DIFFERS
     - 17
     - 四次握手中产生的元素与 (re-)association 后的 request/probe 以及 response/beacon frame 中的信息不同。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。
       -  station 发现四次握手的 IE 与 (re-)association 后的 request/probe 以及 response/beacon frame 中的 IE 不同。
   * - GROUP_CIPHER_INVALID
     - 18
     - 无效组密文。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。
   * - PAIRWISE_CIPHER_INVALID
     - 19
     - 无效成对密文。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。
   * - AKMP_INVALID
     - 20
     - 无效 AKMP。

       对于 ESP station，出现以下情况时报告该代码：
       - 从 AP 接收到该代码。
   * - UNSUPP_RSN_IE_VERSION
     - 21
     - 不支持的 RSNE 版本。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。
   * - INVALID_RSN_IE_CAP
     - 22
     - 无效的 RSNE 性能。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。
   * - 802_1X_AUTH_FAILED
     - 23
     - IEEE 802.1X. authentication 失败。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。

       对于 ESP AP，出现以下情况时将报告该代码：

       - IEEE 802.1X. authentication 失败。
   * - CIPHER_SUITE_REJECTED
     - 24
     - 因安全策略，安全密钥算法套件 (cipher suite) 被拒。

       对于 ESP station，出现以下情况时报告该代码：

       - 从 AP 接收到该代码。
   * - TDLS_PEER_UNREACHABLE
     - 25
     - 通过 TDLS 直连无法到达 TDLS 对端 STA，导致 TDLS 直连中断。
   * - TDLS_UNSPECIFIED
     - 26
     - 不明原因的 TDLS 直连中断。
   * - SSP_REQUESTED_DISASSOC
     - 27
     - association 取消，由于会话被 SSP request 终止。
   * - NO_SSP_ROAMING_AGREEMENT
     - 28
     - association 取消，由于缺乏 SSP 漫游认证。
   * - BAD_CIPHER_OR_AKM
     - 29
     - 请求的服务被拒绝，由于 SSP 密码套件或者 AKM 的需求。
   * - NOT_AUTHORIZED_THIS_LO CATION
     - 30
     - 请求的服务在此位置未得到授权。
   * - SERVICE_CHANGE_PRECLUDES_TS
     - 31
     - TS 被删除，原因是：BSS 服务特性或者运行模式改变导致 Qos AP 缺少足够的带宽给 Qos STA 使用（例如：一个HT BSS 从 40 MHz 的信道切换到 20 MHz 的信道）。
   * - UNSPECIFIED_QOS
     - 32
     - association 取消，由于不明确的 QoS 相关原因。
   * - NOT_ENOUGH_BANDWIDTH
     - 33
     - association 取消，由于 QoS AP 缺少足够的带宽给该 QoS STA 使用。
   * - MISSING_ACKS
     - 34
     - association 取消，原因是：大量的帧需要被确认，但由于 AP 传输或者糟糕的信道条件而没有被确认。
   * - EXCEEDED_TXOP
     - 35
     - association 取消，由于 STA 的传输超过了 TXOPs 的限制。
   * - STA_LEAVING
     - 36
     - 请求 STA 离开了 BSS 或者重置了。
   * - END_BA
     - 37
     - 请求 STA 不再使用该流或者会话。
   * - UNKNOWN_BA
     - 38
     - 请求 STA 使用一种尚未完成的机制接收帧。
   * - TIMEOUT
     - 39
     - 对端 STA 的请求超时。
   * - Reserved
     - 40 ~ 45
     - 根据 IEEE 802.11-2020 协议保留。
   * - PEER_INITIATED
     - 46
     - 在 Disassociation 帧中：已达到授权访问限制。
   * - AP_INITIATED
     - 47
     - 在 Disassociation 帧中：外部服务需求。
   * - INVALID_FT_ACTION_FRAME_COUNT
     - 48
     - 无效的 FT Action 帧计数。
   * - INVALID_PMKID
     - 49
     - 无效的成对主密钥标识符（PMKID）。
   * - INVALID_MDE
     - 50
     - 无效的 MDE。
   * - INVALID_FTE
     - 51
     - 无效的 FTE。
   * - TX_LINK_EST_FAILED
     - 67
     - 在备用信道中建立传输链路失败。
   * - ALTERATIVE_CHANNEL_OCCUPIED
     - 68
     - 备用信道被占用。
   * - BEACON_TIMEOUT
     - 200
     - 乐鑫特有的 Wi-Fi 原因代码： 当 station 连续失去 N 个 beacon，将中断连接并报告该代码。
   * - NO_AP_FOUND
     - 201
     - 乐鑫特有的 Wi-Fi 原因代码： 当 station 未扫描到目标 AP 时，将报告该代码。
   * - AUTH_FAIL
     - 202
     - 乐鑫特有的 Wi-Fi 原因代码： authentication 失败，但并非由超时而引发。
   * - ASSOC_FAIL
     - 203
     - 乐鑫特有的 Wi-Fi 原因代码： association 失败，但并非由 DISASSOC_DUE_TO_INACTIVITY 或 ASSOC_TOOMANY 引发。
   * - HANDSHAKE_TIMEOUT
     - 204
     - 乐鑫特有的 Wi-Fi 原因代码： 握手失败，与 WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT 中失败原因相同。
   * - CONNECTION_FAIL
     - 205
     - 乐鑫特有的 Wi-Fi 原因代码： AP 连接失败。
   * - AP_TSF_RESET
     - 206
     - 乐鑫特有的 Wi-Fi 原因代码：由于 AP 的 TSF 重置导致断开连接。
   * - ROAMING
     - 207
     - 乐鑫特有的 Wi-Fi 原因代码：station 正在漫游到另一个 AP，此原因代码仅供参考，station 将自动切换到另一个 AP。
   * - ASSOC_COMEBACK_TIME_TOO_LONG
     - 208
     - 乐鑫特有的 Wi-Fi 原因代码：当关联响应中的 Assoc 回来时间过长时，将报告此原因代码。
   * - SA_QUERY_TIMEOUT
     - 209
     - 乐鑫特有的 Wi-Fi 原因代码：当 ESP station 发送的 SA 查询未收到 AP 的回复时，将报告此原因代码。
   * - NO_AP_FOUND_SECURITY
     - 210
     - 乐鑫特有的 Wi-Fi 原因代码：当找到符合标识条件（例如 SSID）的 AP，但由于安全配置不兼容导致连接被拒绝时，将报告 NO_AP_FOUND_W_COMPATIBLE_SECURITY。可能的情况包括：

        - 接入点提供 WEP 安全性，但 station 密码不符合 WEP 要求。
        - station 配置为开放模式，但接入点以安全模式广播。
        - 接入点使用企业安全性，但我们未设置相应的企业配置，反之亦然。
        - station 配置为 SAE-PK，但接入点不支持 SAE-PK。
        - station 配置为 SAE-H2E，但 AP 仅支持 WPA3-PSK 或 WPA3-WPA2-PSK。
        - station 配置为安全模式（密码或企业模式），但扫描中找到的 AP 是开放模式。
        - station 配置为 SAE HnP，但 AP 仅支持 H2E。
        - station 禁用 H2E，但 AP 是 WPA3-EXT-PSK，需支持 H2E。
        - 接入点需要 PMF，但 station 未配置为支持 PMF。
        - station 要求 PMF，但接入点未配置为支持 PMF。
        - 接入点使用不支持的组管理/对等加密算法。
        - station 未启用 OWE，但发现的 AP 使用 OWE 模式。
        - 接入点在信标中广播了无效的 RSNXE 信息。
        - 接入点处于独立 BSS 模式。

   * - NO_AP_FOUND_AUTHMODE
     - 211
     - 乐鑫特有的 Wi-Fi 原因代码：当找到符合标识条件（例如 SSID）的 AP，但未满足 wifi_config_t 中设置的身份验证模式阈值时，将报告 NO_AP_FOUND_IN_AUTHMODE_THRESHOLD。
   * - NO_AP_FOUND_RSSI
     - 212
     - 乐鑫特有的 Wi-Fi 原因代码：当找到符合标识条件（例如 SSID）的 AP，但未满足 wifi_config_t 中设置的 RSSI 阈值时，将报告 NO_AP_FOUND_IN_RSSI_THRESHOLD。

与密码错误有关的 Wi-Fi 原因代码
+++++++++++++++++++++++++++++++++

下表罗列了与密码错误相关的 Wi-Fi 原因代码。

.. list-table::
   :header-rows: 1
   :widths: 5 10 40

   * - 原因代码
     - 数值
     - 描述
   * - 4WAY_HANDSHAKE_TIMEOUT
     - 15
     - 四次握手超时。STA 在连接加密的 AP 的时候设置了错误的密码。
   * - NO_AP_FOUND
     - 201
     - 这可能与以下两种场景中的密码错误有关：

       - STA 连接到未加密的 AP 时设置了密码。
       - STA 连接到加密的 AP 时未设置密码。
   * - HANDSHAKE_TIMEOUT
     - 204
     - 四次握手失败。

与低 RSSI 有关的 Wi-Fi 原因代码
+++++++++++++++++++++++++++++++++

下表罗列了与低 RSSI 相关的 Wi-Fi 原因代码。

.. list-table::
   :header-rows: 1
   :widths: 5 10 40

   * - 原因代码
     - 数值
     - 描述
   * - NO_AP_FOUND_IN_RSSI_THRESHOLD
     - 212
     - 低 RSSI 导致 station 无法扫描到目标 AP
   * - HANDSHAKE_TIMEOUT
     - 204
     - 四次握手失败。

.. _wifi-station-connecting-when-multiple-aps-are-found:

{IDF_TARGET_NAME} Wi-Fi Station 在发现多个 AP 时的连接策略
----------------------------------------------------------------------

该场景与 :ref:`{IDF_TARGET_NAME} Wi-Fi Station 连接场景 <wifi-station-connecting-scenario>` 相似，不同之处在于该场景中不会产生 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件，除非 station 无法连接所有找到的 AP。

.. _wifi-reconnect:

Wi-Fi 重新连接
---------------------------

出于多种原因，station 可能会断开连接，例如：连接的 AP 重新启动等。应用程序应负责重新连接。推荐使用的方法为：在接收到 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件后调用函数 :cpp:func:`esp_wifi_connect()`。

但有时，应用程序需要更复杂的方式进行重新连接：

- 如果断开连接事件是由调用函数 :cpp:func:`esp_wifi_disconnect()` 引发的，那么应用程序可能不希望进行重新连接。
- 如果 station 随时可能调用函数 :cpp:func:`esp_wifi_scan_start()` 开始扫描，此时就需要一个更佳的重新连接方法，请参阅 :ref:`连接 Wi-Fi 时扫描 <scan-when-wifi-is-connecting>`。

另一点需要注意的是，如果存在多个具有相同 SSID 的 AP，那么重新连接后可能不会连接到之前的同一个 AP。重新连接时，station 将永远选择最佳的 AP 进行连接。

Wi-Fi beacon 超时
---------------------------

{IDF_TARGET_NAME} 使用 beacon 超时机制检测 AP 是否活跃。如果 station 在 inactive 时间内未收到所连接 AP 的 beacon，将发生 beacon 超时。inactive 时间通过调用函数 :cpp:func:`esp_wifi_set_inactive_time()` 设置。

beacon 超时发生后，station 将向 AP 发送 5 个 probe request，如果仍未从 AP 接收到 probe response 或 beacon，station 将与 AP 断开连接并产生 :ref:`WIFI_EVENT_STA_DISCONNECTED <wifi-event-sta-disconnected>` 事件。

需要注意的是，扫描过程中会重置 beacon 超时所使用的定时器，即扫描过程会影响 :ref:`WIFI_EVENT_STA_BEACON_TIMEOUT <wifi-event-sta-beacon-timeout>` 事件的触发。
